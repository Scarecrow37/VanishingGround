#include "pch.h"
#include "SSAOBlendPass.h"

SSAOBlendPass::SSAOBlendPass() {}

SSAOBlendPass::~SSAOBlendPass() {}

void SSAOBlendPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                               ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    _fxSSAOBlend.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void SSAOBlendPass::AddRenderPassDatas(std::string_view sceneName) {}

void SSAOBlendPass::Begin(ID3D12GraphicsCommandList* commandList) {}

void SSAOBlendPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->RSSetViewports(1, &renderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &renderTarget->GetScissorRect());
    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fxSSAOBlend.GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_fxSSAOBlend.GetRootParameterIndex("AOTexture"),
                                                _sharedRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fxSSAOBlend.GetRootParameterIndex("LightTexture"),
                                                 _meshRenderTarget->GetSRVHandle());
    _ownerScene->_frameQuad->Render(commandList);
    
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->CopyResource(_meshRenderTarget->GetResource(), renderTarget->GetResource());
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    
    Global::multiRenderTargetManager->ReturnRenderTarget(renderTarget);
}

void SSAOBlendPass::End(ID3D12GraphicsCommandList* commandList) {}
