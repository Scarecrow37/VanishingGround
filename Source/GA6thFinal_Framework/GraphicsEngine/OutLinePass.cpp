#include "pch.h"
#include "OutLinePass.h"

OutLinePass::OutLinePass() {}

OutLinePass::~OutLinePass() {}

void OutLinePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void OutLinePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void OutLinePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto& resoultion = Global::device->GetResolution();

    PostProcessData postProcessData{.ScreenSize      = {(float)resoultion.Width, (float)resoultion.Height},
                                    .PostProcessMask = PostProcess::OUTLINE};
    postProcessData.TexelSize = 1.f / postProcessData.ScreenSize;

    auto normal            = Global::multiRenderTargetManager->GetRenderTarget("Normal");
    auto depth             = Global::multiRenderTargetManager->GetRenderTarget("Depth");
    auto customDepthTarget = Global::multiRenderTargetManager->GetRenderTarget("CustomDepth");

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
        
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_6_postProcessData"), 6, &postProcessData, 0);
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("depthTexture"), depth->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("normalTexture"), normal->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("customDepthTexture"), customDepthTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("accumulation"), _ownerScene->_accumulationBuffer->GetUAVHandle());

    _ownerScene->_frameQuad->Render(commandList);
}

void OutLinePass::End(ID3D12GraphicsCommandList* commandList)
{
}