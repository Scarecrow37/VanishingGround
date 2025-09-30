#include "pch.h"
#include "SSGICompositePass.h"
#include "SSGITechnique.h"

SSGICompositePass::~SSGICompositePass() {}

void SSGICompositePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                                   ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);

    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);

    _ssgiTech = dynamic_cast<SSGITechnique*>(ownerTechnique);
}

void SSGICompositePass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    renderTarget->ClearRenderTarget(commandList, 0);

    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &renderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &renderTarget->GetScissorRect());

    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");
    auto        ssgiTexture       = _ssgiTech->_finalGITex;

    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("screenColor"),
                                                _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("screenAlbedo"),
                                                renderTargetGroup[GBuffer::BASECOLOR]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("ssgiTexture"), ssgiTexture->GetSRVHandle());
    _ownerScene->_frameQuad->Render(commandList);

    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->CopyResource(_meshRenderTarget->GetResource(), renderTarget->GetResource());
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    Global::multiRenderTargetManager->ReturnRenderTarget(renderTarget);
}
