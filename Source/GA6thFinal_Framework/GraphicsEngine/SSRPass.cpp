#include "pch.h"
#include "SSRPass.h"

SSRPass::SSRPass() = default;

SSRPass::~SSRPass() = default;

void SSRPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
  
    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};

    _fxSSR.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void SSRPass::AddRenderPassDatas(std::string_view sceneName) 
{
    Global::renderPassDatas->AddRenderPassProperty("SSRPass", SSRPassProperty({0.3f, 0.34f, 200.f,2.f}));
}

void SSRPass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    renderTarget->ClearRenderTarget(commandList, 0);

    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &renderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &renderTarget->GetScissorRect());

    auto        ssrProperty       = std::any_cast<SSRPassProperty>(Global::renderPassDatas->GetRenderPassProperty("SSRPass"));
    const auto& renderTargetGroup = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");
    auto        cameraData        = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();

    commandList->SetGraphicsRootSignature(_fxSSR.GetRootSignature());
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootDescriptorTable(_fxSSR.GetRootParameterIndex("screenColor"), _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fxSSR.GetRootParameterIndex("screenNormal"), renderTargetGroup[GBuffer::NORMAL]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fxSSR.GetRootParameterIndex("screenDepth"), renderTargetGroup[GBuffer::DEPTH]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fxSSR.GetRootParameterIndex("screenORM"), renderTargetGroup[GBuffer::ORM]->GetSRVHandle());
    commandList->SetGraphicsRootConstantBufferView(_fxSSR.GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetGraphicsRoot32BitConstants(_fxSSR.GetRootParameterIndex("bit32_4_ssrProperty"), 4, &ssrProperty, 0);
    _ownerScene->_frameQuad->Render(commandList);

    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->CopyResource(_meshRenderTarget->GetResource(), renderTarget->GetResource());
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    Global::multiRenderTargetManager->ReturnRenderTarget(renderTarget);
}