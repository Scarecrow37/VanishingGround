#include "pch.h"
#include "EndlessGridPass.h"

EndlessGridPass::EndlessGridPass() {}

EndlessGridPass::~EndlessGridPass() {}

void EndlessGridPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    D3D12_RENDER_TARGET_BLEND_DESC rtDesc{};
    rtDesc.BlendEnable               = TRUE;
    rtDesc.SrcBlend                  = D3D12_BLEND_SRC_ALPHA;
    rtDesc.DestBlend                 = D3D12_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp                   = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha             = D3D12_BLEND_ZERO;
    rtDesc.DestBlendAlpha            = D3D12_BLEND_ONE;
    rtDesc.BlendOpAlpha              = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask     = D3D12_COLOR_WRITE_ENABLE_ALL;

    PipelineStateStream pss;
    pss.BlendState                            = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    (&pss.BlendState)->AlphaToCoverageEnable  = FALSE;
    (&pss.BlendState)->IndependentBlendEnable = FALSE;
    (&pss.BlendState)->RenderTarget[0]        = rtDesc;
    pss.RasterizerState                       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode          = D3D12_CULL_MODE_NONE;
    pss.DepthStencilState                     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable     = FALSE;
    pss.PrimitiveTopology                     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                            = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void EndlessGridPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_READ);

    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, &_ownerScene->_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void EndlessGridPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), _ownerScene->_cameraBuffer->GetGPUVirtualAddress());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    commandList->DrawInstanced(4, 1, 0, 0);
}

void EndlessGridPass::End(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_PRESENT);
}