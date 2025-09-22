#include "pch.h"
#include "UI2DPass_OIT.h"
#include "FrameResource.h"
#include "UITechnique_OIT.h"

UI2DPass_OIT::UI2DPass_OIT(const std::vector<UINT>* instanceIDs)
    : UIPassBase_OIT(instanceIDs)
{
}

UI2DPass_OIT::~UI2DPass_OIT() = default;

void UI2DPass_OIT::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase_OIT::Initialize(ownerScene, ownerTechnique, commandList);

    _cameraData.View = XMMatrixTranspose(XMMatrixLookAtLH({0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}));

    PipelineStateStream pss;
    pss.BlendState                   = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState              = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    pss.DepthStencilState            = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.PrimitiveTopology            = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.DSVFormat                    = _ownerScene->_depthStencilView->GetFormat();

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void UI2DPass_OIT::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto depthStencilView = static_cast<UITechnique_OIT*>(_ownerTechnique)->GetDepthStencilView();
    depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    depthStencilView->ClearDepthStencilView(commandList);

    _nodesBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _headBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _atomicCounterBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    UINT value[4] = {UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX};
    _headBuffer->ClearUnorderedAccessView(commandList, value);
    _atomicCounterBuffer->ClearUnorderedAccessView(commandList, Vector4(0, 0, 0, 0));

    const auto& size = Global::device->GetResolution();

    _cameraData.Projection = XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0.f, (float)size.cx, (float)size.cy, 0.f, 0.1f, 1000.f));
    _cameraBuffer->UpdateBuffer(&_cameraData);

    UIPassBase_OIT::UpdateBuffer(commandList);

    commandList->OMSetRenderTargets(0, nullptr, FALSE, &depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void UI2DPass_OIT::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    frameResource->SetFrameResource(FrameResourceType::UI_TRANSFORM, _fx.GetRootParameterIndex("matrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::UI_MATERIAL, _fx.GetRootParameterIndex("material"), commandList);
    
    auto uiMaterialDataBuffer = static_cast<UITechnique_OIT*>(_ownerTechnique)->GetUIMaterialDataBuffer();
    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("uiMaterialData"), uiMaterialDataBuffer->GetGPUVirtualAddress());

    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("IDs"), _instanceIDBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("OITHead"), _headBuffer->GetUAVHandle());
    commandList->SetGraphicsRootUnorderedAccessView(_fx.GetRootParameterIndex("OITNodes"), _nodesBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootUnorderedAccessView(_fx.GetRootParameterIndex("OITCounter"), _atomicCounterBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("textures"), resource);

    _halfQuad->Render(commandList, (UINT)_instanceIDs->size());
}