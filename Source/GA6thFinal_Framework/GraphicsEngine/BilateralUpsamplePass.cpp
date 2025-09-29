#include "pch.h"
#include "BilateralUpsamplePass.h"
#include "SSGITechnique.h"
BilateralUpsamplePass::~BilateralUpsamplePass() {}

void BilateralUpsamplePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
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

void BilateralUpsamplePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    auto renderTarget = _ssgiTech->_finalGITex;
    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    renderTarget->ClearRenderTarget(commandList, 0);

    commandList->OMSetRenderTargets(1, &renderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &renderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &renderTarget->GetScissorRect());

    int currentIndex = _ssgiTech->_currIndex;

    auto                      temporalGITex = _ssgiTech->_GITemporalHalf;
    D3D12_GPU_VIRTUAL_ADDRESS giData        = _ssgiTech->GetConstantBufferView()->GetGPUVirtualAddress();
    const auto&               gbuffers      = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");

    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    commandList->SetPipelineState(_pipelineState.Get());

    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("ssgiData"), giData);
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("screenDepth"),
                                                gbuffers[GBuffer::DEPTH]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("screenNormal"),
                                                gbuffers[GBuffer::NORMAL]->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("ssgiHalf"), temporalGITex->GetSRVHandle());

    _ownerScene->_frameQuad->Render(commandList);

    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void BilateralUpsamplePass::AddRenderPassDatas(std::string_view sceneName)
{
    Global::renderPassDatas->AddRenderPassImage(sceneName, "BilateralUpsamplePass", "SSGIFinalTexture",
                                                _ssgiTech->_finalGITex->GetSRVHandle());
}
