#include "pch.h"
#include "SSRPass.h"

SSRPass::SSRPass() {}

SSRPass::~SSRPass() {}

void SSRPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                         ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    PipelineStateStream pss;
    pss.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    _fxSSR.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void SSRPass::AddRenderPassDatas(std::string_view sceneName) 
{
    Global::renderPassDatas->AddRenderPassProperty(sceneName, "SSRPass", SSRPassProperty({50.f, 0.2f, 0.1f}));
}

void SSRPass::Begin(ID3D12GraphicsCommandList* commandList) {}

void SSRPass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    const auto& ssrProperty       = std::any_cast<const SSRPassProperty>(_ownerScene->GetRenderPassProperty("SSRPass"));
    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("GBuffer");
    auto        cameraData        = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    commandList->RSSetViewports(1, &renderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &renderTarget->GetScissorRect());
    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fxSSR.GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_fxSSR.GetRootParameterIndex("screenColor"),
                                                _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fxSSR.GetRootParameterIndex("screenNormal"),
                                                renderTargetGroup[GBuffer::NORMAL]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fxSSR.GetRootParameterIndex("screenDepth"),
                                                renderTargetGroup[GBuffer::DEPTH]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fxSSR.GetRootParameterIndex("screenORM"),
                                                renderTargetGroup[GBuffer::ORM]->GetSRVHandle());
    commandList->SetGraphicsRootConstantBufferView(_fxSSR.GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetGraphicsRoot32BitConstants(_fxSSR.GetRootParameterIndex("bit32_3_ssrProperty"), 3, &ssrProperty, 0);
    _ownerScene->_frameQuad->Render(commandList);
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->CopyResource(_meshRenderTarget->GetResource(), renderTarget->GetResource());
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    Global::multiRenderTargetManager->ReturnRenderTarget(renderTarget);
}

void SSRPass::End(ID3D12GraphicsCommandList* commandList) {}
