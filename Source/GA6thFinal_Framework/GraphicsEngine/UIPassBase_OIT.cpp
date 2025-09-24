#include "pch.h"
#include "UIPassBase_OIT.h"
#include "StructuredBuffer.h"
#include "Model.h"
#include "UITechnique_OIT.h"

UIPassBase_OIT::UIPassBase_OIT() = default;

UIPassBase_OIT::UIPassBase_OIT(const std::vector<UINT>* instanceIDs)
    : _instanceIDs(instanceIDs)
{
}

UIPassBase_OIT::~UIPassBase_OIT() = default;

void UIPassBase_OIT::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    _instanceIDBuffer = std::make_unique<StructuredBuffer>();
    _instanceIDBuffer->Initialize(sizeof(UINT), 1000);

    _quadModel = Global::resourceManager->LoadResource<Model>(L"HalfQuad");
    _halfQuad  = _quadModel->GetMeshes().front().get();

    _cameraBuffer = std::make_unique<ConstantBufferView>();
    _cameraBuffer->Initialize(sizeof(CameraData));

    _headBuffer          = static_cast<UITechnique_OIT*>(_ownerTechnique)->GetHeadBuffer();
    _nodesBuffer         = static_cast<UITechnique_OIT*>(_ownerTechnique)->GetNodesBuffer();
    _atomicCounterBuffer = static_cast<UITechnique_OIT*>(_ownerTechnique)->GetAtomicCounterBuffer();
}

void UIPassBase_OIT::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto& depthStencilView = _ownerScene->_depthStencilView;
    depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, &depthStencilView->GetDSVHandle());

    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void UIPassBase_OIT::End(ID3D12GraphicsCommandList* commandList)
{    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void UIPassBase_OIT::UpdateBuffer(ID3D12GraphicsCommandList* commandList)
{
    _instanceIDBuffer->CopyStructuredBuffer(commandList, (void*)_instanceIDs->data(), (UINT)_instanceIDs->size());
}