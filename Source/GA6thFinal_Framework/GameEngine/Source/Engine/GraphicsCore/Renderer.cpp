#include "pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "FrameResource.h"
#include "Model.h"
#include "UmScripts.h"

#define SeongU01
#ifdef SeongU01
#include "Box.h"
#include "Sphere.h"
#include "GeoSphere.h"
#include "Cylinder.h"
#include "Grid.h"
#include "Quad.h"
#endif

Renderer::Renderer()
	: _shader(std::make_unique<Shader>())
	, _frameResource(std::make_unique<FrameResource>())
	, _currnetState(0)
{
}

Renderer::~Renderer()
{
}

void Renderer::RegisterRenderQueue(MeshRenderer* component)
{
    auto iter = std::find_if(_components.begin(), _components.end(), [component](const auto& ptr) { return ptr == component; });

    if (iter != _components.end())
    {
        ASSERT(false, L"Renderer::RegisterRenderQueue : Already registered component.");
        return;
    }

    _components.push_back(component);
}

void Renderer::Initialize()
{
	_shader->BeginBuild();
	_shader->LoadShader(L"../Shaders/vs_fr.hlsl", Shader::Type::VS);
	_shader->LoadShader(L"../Shaders/ps_fr.hlsl", Shader::Type::PS);
	_shader->EndBuild();

	CreatePipelineState();

	UmDevice.SetCurrentPipelineState(_pipelineState[_currnetState]);

	// 임시
	_frameResource->Initialize(100, 50);

	CameraData cameraData
	{
		.View = UmMainCamera.GetViewMatrix(),
		.Projection = UmMainCamera.GetProjectionMatrix(),
	};

	UmDevice.CreateConstantBuffer(&cameraData, sizeof(CameraData), _cameraBuffer);
}

void Renderer::Update()
{
    // 비활성된 컴포넌트 제거
    auto first = std::remove_if(_components.begin(), _components.end(), [](const auto& ptr) { return (!ptr->Enable || !ptr->gameObject->ActiveInHierarchy); });
    _components.erase(first, _components.end());

    UmMainCamera.Update();

	UmDevice.ResetCommands();
	UpdateFrameResource();
	UmDevice.ClearBackBuffer(D3D12_CLEAR_FLAG_DEPTH, { 0.5f, 0.5f, 0.5f, 1.f });
}

void Renderer::Render()
{
	ComPtr<ID3D12GraphicsCommandList> commandList = UmDevice.GetCommandList();
	ComPtr<ID3D12RootSignature> rootSignature = _shader->GetRootSignature();

	commandList->SetPipelineState(_pipelineState[_currnetState].Get());
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	
	ComPtr<ID3D12DescriptorHeap> dh = _frameResource->GetDescriptorHeap();
	ID3D12DescriptorHeap* hps[] = { dh.Get(), };

	//디스크립터-힙 설정.
	commandList->SetDescriptorHeaps(_countof(hps), hps);

	//디스크립터-힙에서 첫번째 디스크립터 (배열)주소 획득.
	D3D12_GPU_DESCRIPTOR_HANDLE textures = dh->GetGPUDescriptorHandleForHeapStart();
	
	//ID3DR
	// 현재 타겟의 카메라 버퍼 설정	
	commandList->SetGraphicsRootConstantBufferView(_shader->GetRootSignatureIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());

	commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("objectData"), textures);
	textures.ptr += UmDevice.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("material"), textures);
	textures.ptr += UmDevice.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("textures"), textures);

	UINT ID = 0;
    for (auto& component : _components)
    {
        const auto& model = component->GetModel();
        
        for (auto& mesh : model->GetMeshes())
        {
            commandList->SetGraphicsRoot32BitConstant(_shader->GetRootSignatureIndex("bit32_object"), ID++, 0);
            mesh->Render(commandList.Get());
        }
    }
	
	UmDevice.Flip();
}

HRESULT Renderer::CreatePipelineState()
{
	HRESULT hr = S_OK;

	D3D12_RASTERIZER_DESC rd = {};
	rd.FillMode = D3D12_FILL_MODE_SOLID;
	rd.CullMode = D3D12_CULL_MODE_BACK;
	//rd.FrontCounterClockwise = false;
	rd.DepthClipEnable = TRUE;

	D3D12_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = FALSE;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_DEPTH_STENCIL_DESC  dsd = {};
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	dsd.StencilEnable = FALSE;
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
	psd.pRootSignature = _shader->GetRootSignature().Get();
	psd.VS = _shader->GetShaderByteCode(Shader::Type::VS);
	psd.PS = _shader->GetShaderByteCode(Shader::Type::PS);
	//psd.GS = gs;
	//psd.StreamOutput		= {};
	psd.BlendState = bd;
	psd.SampleMask = UINT_MAX;
	psd.RasterizerState = rd;
	psd.DepthStencilState = dsd;
	psd.InputLayout = _shader->GetInputLayout();
	//psd.IBStripCutValue	= 0;
	psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psd.NumRenderTargets = 1;
	psd.RTVFormats[0] = UmDevice.GetMode().Format;
	psd.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT; // 임시 포맷
	//psd.DSVFormat = 0;						   //DS 포멧. (미지정시 스왑체인에 등록된 DS 버퍼의 포멧 값 사용)
	psd.SampleDesc = { 1, 0 };					   //AA 설정.
	//psd.NodeMask 			= 0;
	//psd.CachedPSO 		= nullptr;
	//psd.Flags 			= D3D12_PIPELINE_STATE_FLAG_NONE;
	
	//첫번째 렌더링 상태 객체 : "Solid" (기본상태)
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();

	hr = device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(_pipelineState[0].GetAddressOf()));
	FAILED_CHECK_BREAK(hr);
	
	psd.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	hr = device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(_pipelineState[1].GetAddressOf()));
	FAILED_CHECK_BREAK(hr);

	return hr;
}

void Renderer::UpdateFrameResource()
{
    CameraData cameraData
    {
        .View       = XMMatrixTranspose(UmMainCamera.GetViewMatrix()),
        .Projection = XMMatrixTranspose(UmMainCamera.GetProjectionMatrix()),
    };

	UmDevice.UpdateBuffer(_cameraBuffer, &cameraData, sizeof(CameraData));

	std::unordered_map<size_t, UINT>         materialPair;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> handles;
    std::vector<XMMATRIX>                    worldMatrixes;
    std::vector<MaterialData>                materialDatas;
    UINT                                     materialID = 0;

    for (auto& component : _components)
    {
        auto& model     = component->GetModel();
        auto& meshes    = model->GetMeshes();
        auto& materials = model->GetMaterials();
         
        XMMATRIX world = XMMatrixTranspose(component->gameObject->transform->GetWorldMatrix());
        UINT size = (UINT)meshes.size();

        for (UINT i = 0; i < size; i++)
        {
            worldMatrixes.emplace_back(world);
            MaterialData materialData{};

            for (UINT j = 0; j < 4; j++)
            {
                if (nullptr == materials[i][j])
                    continue;

                auto iter = materialPair.find(materials[i][j]->GetHandle().ptr);
                if (iter == materialPair.end())
                {
                    materialPair.emplace(materials[i][j]->GetHandle().ptr, materialID);
                    materialData.ID[j] = materialID++;
                    handles.push_back(materials[i][j]->GetHandle());
                }
                else
                {
                    materialData.ID[j] = iter->second;
                }
            }

            materialDatas.push_back(materialData);
        }
    }

	UINT size = static_cast<UINT>(worldMatrixes.size());
    _frameResource->CopyStructuredBuffer(UmDevice.GetCommandList().Get(), worldMatrixes.data(), size * sizeof(ObjectData), FrameResource::Type::TRANSFORM);
	_frameResource->CopyStructuredBuffer(UmDevice.GetCommandList().Get(), materialDatas.data(), size * sizeof(MaterialData), FrameResource::Type::MATERIAL);
	_frameResource->CopyDescriptors(handles);
}