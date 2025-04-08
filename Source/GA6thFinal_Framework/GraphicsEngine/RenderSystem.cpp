#include "pch.h"
#include "RenderSystem.h"

#include "DX11Renderer.h"

#include "StaticMeshRenderer.h"
#include "SkeletalMeshRenderer.h"
#include "SkyBoxRenderer.h"

#include "Sampler.h"
#include "TextSystem.h"
#include "SpriteSystem.h"
#include "UnlitSystem.h"
#include "StructuredBuffer.h"

StructuredBuffer*	g_pStructuredBuffer;
ConstantBuffer*		g_pConstantBuffer;
GraphicDevice*		g_pGraphicDevice;
Sampler*			g_pSampler;
ViewManagement*		g_pViewManagement;
RenderGroup*		g_pRenderGroup;
StateManagement*	g_pStateManagement;
Quad*				g_pQuad;
Sprite*				g_pSprite;
float				g_width;
float				g_height;
XMMATRIX			g_orthoGraphic;

void RenderSystem::Initialize(const GE::RENDERER_DESC* pDesc)
{
	g_pResourceMgr = new ResourceManager;
	g_shaderRootPath = pDesc->shaderRootPath;
	g_width = pDesc->width;
	g_height = pDesc->height;

	switch (pDesc->type)
	{
	case GE::RENDERER_DESC::Type::DX11:
		InitializeDX11(pDesc->hWnd, pDesc->isFullScreen, pDesc->maxLayer);
		break;
	case GE::RENDERER_DESC::Type::DX12:
		InitializeDX12();
		break;
	case GE::RENDERER_DESC::Type::DXR:
		InitializeDXR();
		break;
	}


	_pSwapChain = g_pGraphicDevice->GetSwapChain();
}

void RenderSystem::Release()
{
	SafeRelease(g_pStateManagement);
	SafeRelease(g_pResourceMgr);
	SafeRelease(_pPostProcessSystem);
	SafeRelease(_pRenderer);
	SafeRelease(_pTextSystem);
	SafeRelease(_pSpriteSystem);
	SafeRelease(_pUnlitSystem);
	SafeRelease(g_pStructuredBuffer);
	SafeRelease(g_pConstantBuffer);
	SafeRelease(g_pSampler);
	SafeRelease(g_pViewManagement);
	SafeRelease(g_pRenderGroup);
	SafeRelease(g_pQuad);
	SafeRelease(g_pSprite);
	SafeRelease(g_pGraphicDevice);

	delete this;
}

void RenderSystem::Render()
{
	//_pRenderer->Render();
	//_pUnlitSystem->Render();
	//_pPostProcessSystem->Render();
	//_pSpriteSystem->Render();
	//_pTextSystem->Render();

	_pSwapChain->Present(0, 0);

    //Imgui를 위한 임시
    auto* pBackBuffer = g_pGraphicDevice->GetBackBuffer();
    auto* pDeviceContext = g_pGraphicDevice->GetDeviceContext();
    pDeviceContext->OMSetRenderTargets(1, &pBackBuffer, nullptr);
    pDeviceContext->ClearRenderTargetView(pBackBuffer, COLOR_ZERO);
}

void RenderSystem::GetTextSystem(GE::ITextSystem** ppTextSystem)
{	
	(*ppTextSystem) = _pTextSystem;
}

void RenderSystem::GetPostProcessSystem(GE::IPostProcessSystem** ppPostProcessSystem)
{
	(*ppPostProcessSystem) = _pPostProcessSystem;
}

void RenderSystem::GetSpriteSystem(GE::ISpriteSystem** ppSpriteSystem)
{
	(*ppSpriteSystem) = _pSpriteSystem;
}

void RenderSystem::GetUnlitSystem(GE::IUnlitSystem** ppUnlitSystem)
{
	(*ppUnlitSystem) = _pUnlitSystem;
}

void RenderSystem::CreateMeshRenderer(GE::IMeshRenderer** ppComponent, const GE::MESH_RENDERER_DESC* pDesc)
{
	MeshRenderer* pMeshRenderer = nullptr;

	switch (pDesc->type)
	{
	case GE::MESH_RENDERER_DESC::Type::Static:
		pMeshRenderer = new StaticMeshRenderer;
		break;
	case GE::MESH_RENDERER_DESC::Type::Skeletal:
		pMeshRenderer = new SkeletalMeshRenderer;
		break;
	case GE::MESH_RENDERER_DESC::Type::SkyBox:
		pMeshRenderer = new SkyBoxRenderer;
		break;
	}

	pMeshRenderer->Initialize(pDesc->filePath);
	(*ppComponent) = pMeshRenderer;
}

void RenderSystem::RegisterRenderQueue(GE::IMeshRenderer* pComponent, GE::Matrix4x4* pMatrix)
{
	g_pRenderGroup->RegisterRenderQueue(pComponent, pMatrix);
}

void RenderSystem::UnRegisterRenderQueue(GE::IMeshRenderer* pComponent)
{
	g_pRenderGroup->UnRegisterRenderQueue(pComponent);
}

void RenderSystem::InitializeDX11(HWND hWnd, bool isFullScreen, const unsigned int maxLayer)
{
	g_pGraphicDevice = new GraphicDevice;
	g_pGraphicDevice->Initialize(hWnd, isFullScreen);
	
	g_pViewManagement = new ViewManagement;
	g_pStructuredBuffer = new StructuredBuffer;
	g_pConstantBuffer = new ConstantBuffer;
	g_pRenderGroup = new RenderGroup;

	g_pStateManagement = new StateManagement;
	g_pStateManagement->Initialize();

	g_pSampler = new Sampler;
	g_pSampler->Initialize();
	
	g_pQuad = new Quad;
	g_pQuad->Initialize();

	g_pSprite = new Sprite;
	g_pSprite->Initialize();

	_pRenderer = new DX11Renderer;
	_pRenderer->Initialize();

	_pTextSystem = new TextSystem;
	_pTextSystem->Initialize();

	_pPostProcessSystem = new PostProcessSystem;
	_pPostProcessSystem->Initialize();

	_pSpriteSystem = new SpriteSystem;
	_pSpriteSystem->Initialize();

	_pUnlitSystem = new UnlitSystem;
	_pUnlitSystem->Initialize();

	g_orthoGraphic = XMMatrixTranspose(XMMatrixOrthographicLH(g_width, g_height, 1.f, 100.f));
}

void RenderSystem::InitializeDX12()
{
}

void RenderSystem::InitializeDXR()
{
}