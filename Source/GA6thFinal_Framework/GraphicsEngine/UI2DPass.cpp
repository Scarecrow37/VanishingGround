#include "pch.h"
#include "UI2DPass.h"
#include "FrameResource.h"
#include "UITechnique.h"

UI2DPass::UI2DPass(const std::vector<UINT>& instanceIDs)
    : UIPassBase(instanceIDs)
{
}

UI2DPass::~UI2DPass() = default;

void UI2DPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    _cameraData.View = XMMatrixTranspose(XMMatrixLookAtLH({0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}));

    D3D12_RENDER_TARGET_BLEND_DESC rtDesc{};
    rtDesc.BlendEnable           = TRUE;
    rtDesc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
    rtDesc.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp               = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha         = D3D12_BLEND_ONE;
    rtDesc.DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    PipelineStateStream pss;
    pss.BlendState                            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    (&pss.BlendState)->AlphaToCoverageEnable  = FALSE;
    (&pss.BlendState)->IndependentBlendEnable = FALSE;
    (&pss.BlendState)->RenderTarget[0]        = rtDesc;
    pss.RasterizerState                       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode          = D3D12_CULL_MODE_NONE;
    pss.DepthStencilState                     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    pss.PrimitiveTopology                     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                            = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    pss.DSVFormat                             = _ownerScene->_depthStencilView->GetFormat();

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void UI2DPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    const auto& size = Global::device->GetResolution();

    _cameraData.Projection = XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0.f, (float)size.cx, (float)size.cy, 0.f, 0.1f, 1000.f));
    _cameraBuffer->UpdateBuffer(&_cameraData);

    UIPassBase::UpdateBuffer(commandList);

    auto depthStencilView = static_cast<UITechnique*>(_ownerTechnique)->GetDepthStencilView();
    depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    depthStencilView->ClearDepthStencilView(commandList);

    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, &depthStencilView->GetDSVHandle());

    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void UI2DPass::Draw(ID3D12GraphicsCommandList* commandList)
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

    _halfQuad->Render(commandList, (UINT)_instanceIDs.size());
}