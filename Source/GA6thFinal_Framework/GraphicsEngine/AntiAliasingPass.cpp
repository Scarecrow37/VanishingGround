#include "pch.h"
#include "AntiAliasingPass.h"

void AntiAliasingPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                                  ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene,ownerTechnique,commandList);

    PipelineStateStream pss;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void AntiAliasingPass::AddRenderPassDatas(std::string_view sceneName) 
{
    Global::renderPassDatas->AddRenderPassProperty("AntiAliasingPass", FXAAProperty(1.f, 0.25f, 0.001f));
}
void AntiAliasingPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    auto resolution = Global::device->GetResolution();

    auto fxaaProperty = std::any_cast<FXAAProperty>(Global::renderPassDatas->GetRenderPassProperty("AntiAliasingPass"));
    FXAAData fxaaData;
    fxaaData.QualityEdgeDetectionThreshold = fxaaProperty.QualityEdgeDetectionThreshold;
    fxaaData.QualityMinimumEdgeThreshold   = fxaaProperty.QualityMinimumEdgeThreshold;
    fxaaData.QualitySubpixel               = fxaaProperty.QualitySubpixel;
    fxaaData.InverseResolution             = Vector2(1.f / resolution.cx, 1.f / resolution.cy);
    
    auto renderTarget = Global::multiRenderTargetManager->GetAvailableRenderTarget();
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    renderTarget->ClearRenderTarget(commandList, 0);
    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &renderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &renderTarget->GetScissorRect());

    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_5_fxaaProperty"), 5, &fxaaData, 0);
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("screenColor"), _meshRenderTarget->GetSRVHandle());
    _ownerScene->_frameQuad->Render(commandList);

    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);
    commandList->CopyResource(_meshRenderTarget->GetResource(), renderTarget->GetResource());
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    Global::multiRenderTargetManager->ReturnRenderTarget(renderTarget);
}
