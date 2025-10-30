#include "pch.h"
#include "UIPassBase.h"
#include "FrameResource.h"
#include "Model.h"
#include "StructuredBuffer.h"
#include "UITechnique.h"

UIPassBase::UIPassBase() = default;

UIPassBase::UIPassBase(const std::vector<UINT>* instanceIDs)
    : _instanceIDs(instanceIDs)
{
}

UIPassBase::~UIPassBase() = default;

void UIPassBase::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    _instanceIDBuffer = std::make_unique<StructuredBuffer>();
    _instanceIDBuffer->Initialize(sizeof(UINT), 1000);

    _quadModel = Global::resourceManager->LoadResource<Model>(L"HalfQuad");
    _halfQuad  = _quadModel->GetMeshes().front().get();

    PipelineStateStream pss;
    pss.BlendState                   = CD3DX12_BLEND_DESC(CommonStates::NonPremultiplied);
    pss.RasterizerState              = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    pss.DepthStencilState            = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.PrimitiveTopology            = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                   = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    pss.DSVFormat                    = _ownerScene->_depthStencilView->GetFormat();

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);

    pss.BlendState = CD3DX12_BLEND_DESC(CommonStates::AlphaBlend);

    _fxSDF.SetPipelineStateStream(pss);
    _sdfPipelineState = Global::pipelineStateManager->GetPipelineState(pss);

    _depthStencilView = static_cast<UITechnique*>(_ownerTechnique)->GetDepthStencilView();
}

void UIPassBase::Begin(ID3D12GraphicsCommandList* commandList)
{
    UIPassBase::UpdateBuffer(commandList);

    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, &_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void UIPassBase::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    frameResource->SetFrameResource(FrameResourceType::UI_TRANSFORM, _fx.GetRootParameterIndex("ui_matrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::UI_MATERIAL, _fx.GetRootParameterIndex("material"), commandList);

    auto uiMaterialDataBuffer = static_cast<UITechnique*>(_ownerTechnique)->GetUIMaterialDataBuffer();
    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("uiMaterialData"), uiMaterialDataBuffer->GetGPUVirtualAddress());

    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("IDs"), _instanceIDBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("textures"), resource);

    _halfQuad->Render(commandList, (UINT)_instanceIDs->size());
}

void UIPassBase::UpdateBuffer(ID3D12GraphicsCommandList* commandList)
{
    _instanceIDBuffer->CopyStructuredBuffer(commandList, (void*)_instanceIDs->data(), (UINT)_instanceIDs->size());
}