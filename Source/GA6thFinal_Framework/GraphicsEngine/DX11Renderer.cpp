#include "pch.h"
#include "DX11Renderer.h"

// Resource
#include "Material.h"
#include "Mesh.h"
#include "Model.h"
#include "VIBuffer.h"

// Shader
#include "PixelShader.h"
#include "VertexShader.h"

// Component
#include "MeshRenderer.h"
#include "SkeletalMeshRenderer.h"
#include "SkyBoxRenderer.h"
#include "Animator.h"
#include "Camera.h"
#include "Light.h"

// Filters
#include "Filter.h"
#include "SSAO.h"

// System
#include "CameraSystem.h"
#include "LightSystem.h"
#include "StructuredBuffer.h"

void DX11Renderer::Initialize()
{
	_pDeviceContext = g_pGraphicDevice->GetDeviceContext();

	_matrices.resize(MAX_DRAW_OBJECT);
	_boneMatrices.resize(MAX_DRAW_OBJECT * MAX_BONE_MATRIX);
	_directionalLights.resize(MAX_DIRECTIONAL_LIGHT);
	_pointLights.resize(MAX_POINT_LIGHT);

	// Viewport
	_viewport.MaxDepth = 1.f;	

	SetViewport(g_width, g_height);

	// Shader
	InitShaders();

	// MRT
	InitMRT();

	// DepthStencil
	InitDepthStencil();

	// StructuredBuffer
	InitStructuredBuffer();

	// Optional
	InitOptional();
	
	// State
	InitState();

	// Filters
	InitFilters();
}

void DX11Renderer::Render()
{	    
	const auto& renderData = g_pRenderGroup->GetRenderDatas();
	
	std::list<DrawData> alphaMeshes[ZWrite_End], noneAlphaMeshs;
	std::list<std::pair<unsigned int, SkyBoxRenderer*>> skyBoxes;

	unsigned int ID = 0;
	for (const auto& [component, matrix] : renderData)
	{
		if (!component->IsActiveDraw())
			continue;

		auto& model = component->GetModel();
		_matrices[ID] = XMMatrixTranspose(*matrix);

		switch (component->GetType())
		{
		case MeshType::Skeletal:
		{
			Animator* pAnimator = static_cast<SkeletalMeshRenderer*>(component)->GetAnimator();
			memcpy(&_boneMatrices[ID * MAX_BONE_MATRIX], pAnimator->GetAnimationTransform(), sizeof(Matrix) * MAX_BONE_MATRIX);
			break;
		}
		case MeshType::SkyBox:
			skyBoxes.emplace_back(ID, static_cast<SkyBoxRenderer*>(component));
			continue;
		}

		const unsigned int mask = component->GetPostEffectFlag();

		for (auto& mesh : model->_meshs)
		{			
			if (mesh->_pMaterial->IsAlpha())
			{
				if (component->IsZWrite())
					alphaMeshes[ZWrite::On].emplace_back(ID, mask, mesh);
				else
					alphaMeshes[ZWrite::Off].emplace_back(ID, mask, mesh);
			}
			else
			{
				noneAlphaMeshs.emplace_back(ID, mask, mesh);
			}
		}

		ID++;
	}

	Camera* pCamera = g_pCameraSystem->GetCurrentCamera();
	XMMATRIX view = XMMatrixIdentity();

	if (nullptr != pCamera)
		view = pCamera->GetViewMatrix();

	alphaMeshes[On].sort([this, view](const DrawData& data0, const DrawData& data1)
		{
			Matrix m0 = XMMatrixTranspose(_matrices[data0.modelID]) * view;
			Matrix m1 = XMMatrixTranspose(_matrices[data1.modelID]) * view;
			return m0.Translation().z > m1.Translation().z;
		});

	auto& lights = g_pLightSystem->GetLights();
	unsigned int numLight[GE::ILight::End]{};
	
	for (auto& light : lights)
	{
		if (!light->GetActive())
			continue;

		switch (light->GetType())
		{
		case GE::ILight::Directional:
			if (numLight[GE::ILight::Directional] >= MAX_DIRECTIONAL_LIGHT)
				continue;
			_directionalLights[numLight[GE::ILight::Directional]++] = light->_lightData;
			break;
		case GE::ILight::Point:
			if (numLight[GE::ILight::Point] >= MAX_POINT_LIGHT)
				continue;
			_pointLights[numLight[GE::ILight::Point]++] = light->_lightData;
			break;
		case GE::ILight::Spot:
			break;
		}
	}

	// Lights
	g_pConstantBuffer->UpdateConstantBuffer(L"DirectionalLights", _directionalLights.data());
	g_pConstantBuffer->UpdateConstantBuffer(L"PointLights", _pointLights.data());
	g_pConstantBuffer->UpdateConstantBuffer(L"NumLight", numLight);

	// Matrices
	g_pStructuredBuffer->UpdateStructuredBuffer(L"WorldMatrices", _matrices.data());
	g_pStructuredBuffer->UpdateStructuredBuffer(L"BoneMatrices", _boneMatrices.data());
	g_pStructuredBuffer->SetStructuredBuffer(L"WorldMatrices", ShaderType::Vertex, 0, 1);
	g_pStructuredBuffer->SetStructuredBuffer(L"BoneMatrices", ShaderType::Vertex, 1, 1);

	// ResetState
	_pDeviceContext->OMSetDepthStencilState(nullptr, 0XFF);

	ShadowPass();
	GBufferPass(noneAlphaMeshs);
	LigthingPass(skyBoxes.front().second, alphaMeshes, skyBoxes);
	//SkyBoxPass(skyBoxes);
}

void DX11Renderer::SetViewport(float width, float height)
{
	_viewport.Width = width;
	_viewport.Height = height;
	_pDeviceContext->RSSetViewports(1, &_viewport);
}

void DX11Renderer::ShadowPass()
{
	Camera* pCamera = g_pCameraSystem->GetCurrentCamera();
	Light* pMainLight = g_pLightSystem->GetMainLight();

	float distance = 1000.f;

	XMVECTOR direction = XMVector3Normalize(-pMainLight->_lightData.data);
	XMVECTOR lightPosition = direction * 2200.f + XMVectorSet(0.f, 2000.f, 0.f, 0.f);
	XMVECTOR lightTarget = XMVectorZero();
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX shadowView = XMMatrixLookAtLH(lightPosition, lightTarget, lightUp);

	float viewWidth = SHADOW_WIDTH;
	float viewHeight = SHADOW_HEIGHT;
	float nearPlane = distance;
	float farPlane = 7000.f;
	
	ViewProjection vp{};
	CameraDesc cameraDesc{};

	//XMMATRIX shadowProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1, nearPlane, farPlane);
	XMMATRIX shadowProjection = XMMatrixOrthographicLH(SHADOW_WIDTH, SHADOW_HEIGHT, nearPlane, farPlane);
	
	vp.shadowVP = XMMatrixTranspose(shadowView * shadowProjection);
	if (pCamera)
	{
		vp.vp = pCamera->GetViewMatrix() * pCamera->GetProjectionMatrix();
		cameraDesc.cameraPosition = pCamera->GetPosition();
	}
	
	cameraDesc.vpInvers = XMMatrixTranspose(XMMatrixInverse(nullptr, vp.vp));
	vp.vp = XMMatrixTranspose(vp.vp);

	g_pConstantBuffer->UpdateConstantBuffer(L"ViewProjection", &vp);
	g_pConstantBuffer->UpdateConstantBuffer(L"CameraDesc", &cameraDesc);

	SetViewport(SHADOW_WIDTH, SHADOW_HEIGHT);

	ClearBindResource(_pDeviceContext, 10, 1);
	_pDeviceContext->OMSetRenderTargets(0, nullptr, _pShadowDSV);
	_pDeviceContext->ClearDepthStencilView(_pShadowDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	_pDeviceContext->PSSetShader(nullptr, nullptr, 0);

	const auto& renderDatas = g_pRenderGroup->GetRenderDatas();
	auto prevType = MeshType::End;
	unsigned int offset[2]{ 0, MAX_BONE_MATRIX };

	unsigned int ID = 0;
	for (auto& [component, matrix] : renderDatas)
	{
		if (!component->IsActiveShadow())
			continue;

		auto type = component->GetType();
		if (type == MeshType::SkyBox)
			continue;

		if (prevType != type)
		{
			_vsShadow[(int)type]->SetVertexShader();
			prevType = type;
		}

		std::shared_ptr<Model>& model = component->GetModel();

		offset[0] = ID++;
		for (auto& mesh : model->_meshs)
		{
			g_pConstantBuffer->UpdateConstantBuffer(L"ModelIndex", &offset);
			mesh->_pVIBuffer->SetParameters(_pDeviceContext);
			mesh->_pVIBuffer->DrawIndexed(_pDeviceContext);
		}
	}

	// Directional Pass
	// Point Pass
	// Spot Pass
	_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	_pDeviceContext->PSSetShaderResources(10, 1, &_pShadowSRV);

	SetViewport(g_width, g_height);
}

void DX11Renderer::GBufferPass(std::list<DrawData>& noneAlphaMeshes)
{
	auto& [RTVs, SRVs] = g_pViewManagement->GetRenderTargetGroup(L"G-Buffer");
	unsigned int numTargets = (unsigned int)RTVs.size();
	
	auto* blendState = g_pStateManagement->GetBlendState(L"NoneBlend");
	_pDeviceContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
	_pDeviceContext->OMSetRenderTargets(numTargets, RTVs.data(), _pDefaultDSV);
	_pDeviceContext->ClearDepthStencilView(_pDefaultDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	for (unsigned int i = 0; i < numTargets; i++) _pDeviceContext->ClearRenderTargetView(RTVs[i], COLOR_ZERO);

	RenderMesh(noneAlphaMeshes, _psGBuffer);

	_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void DX11Renderer::LigthingPass(SkyBoxRenderer* pSkyBox, std::list<DrawData>* alphaMeshes, std::list<std::pair<unsigned int, SkyBoxRenderer*>>& skyBoxes)
{
	pSkyBox->SetParameter(_pDeviceContext, 11);
	
	auto* blendState = g_pStateManagement->GetBlendState(L"AlphaBlend");
	_pDeviceContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
	
	DeferredLighting();

	auto& [RTVs, SRVs] = g_pViewManagement->GetRenderTargetGroup(L"Forward");

	//_pDeviceContext->OMSetDepthStencilState(_pAlphaDepthState, 1);
	_pDeviceContext->OMSetRenderTargets((unsigned int)RTVs.size(), RTVs.data(), _pDefaultDSV);

	SkyBoxPass(skyBoxes);

	//_pDeviceContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);

	// zWrite
	ForwardLigthing(alphaMeshes[On]);

	// zNoneWrite
	//_pDeviceContext->OMSetDepthStencilState(_pAlphaDepthState, 1);
	ForwardLigthing(alphaMeshes[Off]);
	//_pDeviceContext->OMSetDepthStencilState(nullptr, 1);

	//AmbientOcclusion();
	_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void DX11Renderer::SkyBoxPass(std::list<std::pair<unsigned int, SkyBoxRenderer*>>& skyBoxes)
{
	Camera* pCamera = g_pCameraSystem->GetCurrentCamera();
	if (nullptr == pCamera) return;	

	Matrix view = pCamera->GetViewMatrix();
	
	view.m[3][0] = 0.f;
	view.m[3][1] = 0.f;
	view.m[3][2] = 0.f;
	Matrix skyVP = (view * pCamera->GetProjectionMatrix()).Transpose();

	g_pConstantBuffer->UpdateConstantBuffer(L"SkyVP", &skyVP);

	auto& [RTVs, SRVs] = g_pViewManagement->GetRenderTargetGroup(L"Forward");
	auto* blendState = g_pStateManagement->GetBlendState(L"NoneBlend");

	_pDeviceContext->OMSetRenderTargets((unsigned int)RTVs.size(), RTVs.data(), _pDefaultDSV);
	_pDeviceContext->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
	_pDeviceContext->RSSetState(_pRSSkyBoxState);

	unsigned int offset[2]{ 0, MAX_BONE_MATRIX };
	unsigned int layerMask = 0;	
	for (auto& [ID, component] : skyBoxes)
	{
		offset[0] = ID;
		layerMask = component->GetPostEffectFlag();
		g_pConstantBuffer->UpdateConstantBuffer(L"ModelIndex", &offset);
		g_pConstantBuffer->UpdateConstantBuffer(L"LayerMask", &layerMask);
		component->Render(_pDeviceContext);
	}

	_pDeviceContext->RSSetState(nullptr);
	_pDeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	//_pDeviceContext->OMSetDepthStencilState(nullptr, 0);
}

void DX11Renderer::DeferredLighting()
{
	auto& [G_Buffer_RTVs, G_Buffer_SRVs] = g_pViewManagement->GetRenderTargetGroup(L"G-Buffer");
	auto& [Deferred_RTVs, Deferred_SRVs] = g_pViewManagement->GetRenderTargetGroup(L"Deferred");

	_pDeviceContext->OMSetRenderTargets((unsigned int)Deferred_RTVs.size(), Deferred_RTVs.data(), nullptr);
	for (size_t i = 0; i < Deferred_RTVs.size(); i++) _pDeviceContext->ClearRenderTargetView(Deferred_RTVs[i], COLOR_ZERO);	

	// Diffuse, Normal, RMA, Emissive, ShadowPosition
	_pDeviceContext->PSSetShaderResources(0, 5, G_Buffer_SRVs.data());

	// Depth
	_pDeviceContext->PSSetShaderResources(5, 1, &_pDefaultSRV);

	_psDeferredLighting->SetPixelShader();
	g_pQuad->Render();

	ClearBindResource(_pDeviceContext, 0, 6);
}

void DX11Renderer::ForwardLigthing(std::list<DrawData>& alphaMeshes)
{	
	RenderMesh(alphaMeshes, _psForwardLighting);	
}

void DX11Renderer::AmbientOcclusion()
{
	_pSSAO->Render();
}

void DX11Renderer::RenderMesh(std::list<DrawData>& renderData, std::shared_ptr<PixelShader>& pixelShader)
{
	pixelShader->SetPixelShader();

	unsigned int offset[2]{ 0, MAX_BONE_MATRIX };
	unsigned int layerMask = 0;
	for (auto& [ID, mask, mesh] : renderData)
	{
		offset[0] = ID;
		layerMask = mask;
		g_pConstantBuffer->UpdateConstantBuffer(L"ModelIndex", &offset);
		g_pConstantBuffer->UpdateConstantBuffer(L"LayerMask", &layerMask);

		mesh->SetVertexShader();
		mesh->_pVIBuffer->SetParameters(_pDeviceContext);
		mesh->_pMaterial->SetParameters(pixelShader);
		mesh->_pVIBuffer->DrawIndexed(_pDeviceContext);
	}
}

void DX11Renderer::InitState()
{
	g_pStateManagement->AddBlendState(L"AlphaBlend", StateManagement::Type::AlphaBlend);
	g_pStateManagement->AddBlendState(L"NoneBlend", StateManagement::Type::NoneBlend);
		
	ID3D11Device* pDevice = g_pGraphicDevice->GetDevice();

	D3D11_RASTERIZER_DESC rsDesc
	{
		.FillMode = D3D11_FILL_SOLID,
		.CullMode = D3D11_CULL_NONE,
		.DepthClipEnable = FALSE,
	};

	pDevice->CreateRasterizerState(&rsDesc, &_pRSSkyBoxState);

	D3D11_DEPTH_STENCIL_DESC desc = {};
	desc.DepthEnable = true;  // 깊이 테스트 활성화
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 깊이 버퍼 기록 OFF
	desc.DepthFunc = D3D11_COMPARISON_LESS; // 기본적으로 가까운 것 우선

	pDevice->CreateDepthStencilState(&desc, &_pAlphaDepthState);
}

void DX11Renderer::InitOptional()
{
	Vector4 texel = { 1.f / SHADOW_WIDTH, 0.f, 0.f, 0.f };
	g_pConstantBuffer->UpdateConstantBuffer(L"OptionalDesc", &texel);
}

void DX11Renderer::InitMRT()
{
	// Diffuse, Normal, RMA, Emissive, ShadowPosition;

	g_pViewManagement->AddRenderTargetView(L"Base", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"Diffuse", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"Normal", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"RMA", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"Emissive", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"ShadowPosition", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"Ambient", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"LayerMask", Vector2(g_width, g_height), DXGI_FORMAT_R32_UINT);	

	g_pViewManagement->AddRenderTargetGroup(L"G-Buffer", L"Diffuse");
	g_pViewManagement->AddRenderTargetGroup(L"G-Buffer", L"Normal");
	g_pViewManagement->AddRenderTargetGroup(L"G-Buffer", L"RMA");
	g_pViewManagement->AddRenderTargetGroup(L"G-Buffer", L"Emissive");
	g_pViewManagement->AddRenderTargetGroup(L"G-Buffer", L"ShadowPosition");
	g_pViewManagement->AddRenderTargetGroup(L"G-Buffer", L"LayerMask");

	g_pViewManagement->AddRenderTargetGroup(L"Deferred", L"Base");
	g_pViewManagement->AddRenderTargetGroup(L"Deferred", L"Ambient");

	g_pViewManagement->AddRenderTargetGroup(L"Forward", L"Base");
	g_pViewManagement->AddRenderTargetGroup(L"Forward", L"Ambient");
	g_pViewManagement->AddRenderTargetGroup(L"Forward", L"Normal");
	g_pViewManagement->AddRenderTargetGroup(L"Forward", L"LayerMask");

	g_pViewManagement->AddRenderTargetView(L"Blend0", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"Blend1", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"Blend2", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"Blend3", Vector2(g_width, g_height));

	g_pViewManagement->AddRenderTargetGroup(L"Blend", L"Blend0");
	g_pViewManagement->AddRenderTargetGroup(L"Blend", L"Blend1");

	g_pViewManagement->AddRenderTargetGroup(L"Blend2", L"Blend2");
	g_pViewManagement->AddRenderTargetGroup(L"Blend2", L"Blend3");	

	g_pViewManagement->AddMipMapRenderTargetView(L"MipMap", Vector2(g_width, g_height));
	g_pViewManagement->AddRenderTargetView(L"PostProcess", Vector2(g_width, g_height));
}

void DX11Renderer::InitShaders()
{
	_vsShadow[MeshType::Static] = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/ShadowVS.cso");
	_vsShadow[MeshType::Skeletal] = g_pResourceMgr->LoadResource<VertexShader>(L"Assets/Shaders/ShadowVS_Skeletal.cso");	

	_psForwardLighting = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/ForwardLightingPS.cso");
	_psDeferredLighting = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/DeferredLightingPS.cso");
	_psGBuffer = g_pResourceMgr->LoadResource<PixelShader>(L"Assets/Shaders/GBufferPS.cso");	
}

void DX11Renderer::InitDepthStencil()
{
	D3D11_TEXTURE2D_DESC textureDesc
	{
		.Width = (unsigned int)g_width,
		.Height = (unsigned int)g_height,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R32_TYPELESS,
		.SampleDesc {.Count = 1 },
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
	};

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc
	{
		.Format = DXGI_FORMAT_D32_FLOAT,
		.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
	};
	
	g_pViewManagement->AddDepthStencilView(L"DefaultDepth", textureDesc, dsvDesc);

	textureDesc.Width = (unsigned int)SHADOW_WIDTH;
	textureDesc.Height = (unsigned int)SHADOW_HEIGHT;
	//textureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;

	g_pViewManagement->AddDepthStencilView(L"ShadowDepth", textureDesc, dsvDesc);

	_pDefaultDSV = g_pViewManagement->GetDepthStencilView(L"DefaultDepth");
	_pDefaultSRV = g_pViewManagement->GetShaderResourceView(L"DefaultDepth");
	_pShadowDSV = g_pViewManagement->GetDepthStencilView(L"ShadowDepth");
	_pShadowSRV = g_pViewManagement->GetShaderResourceView(L"ShadowDepth");
}

void DX11Renderer::InitStructuredBuffer()
{
	g_pStructuredBuffer->AddStructuredBuffer(L"WorldMatrices", sizeof(XMMATRIX) * MAX_DRAW_OBJECT, MAX_DRAW_OBJECT);
	g_pStructuredBuffer->AddStructuredBuffer(L"BoneMatrices", sizeof(XMMATRIX) * MAX_DRAW_OBJECT * MAX_BONE_MATRIX, MAX_DRAW_OBJECT * MAX_BONE_MATRIX);
}

void DX11Renderer::InitFilters()
{	
	_pSSAO = new SSAO;
	_pSSAO->Initialize();
}

void DX11Renderer::Free()
{
	SafeRelease(_pSSAO);
	SafeRelease(_pRSSkyBoxState);	
	SafeRelease(_pAlphaDepthState);
}