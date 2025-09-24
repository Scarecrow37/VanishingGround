#include "pch.h"
#include "BlendPass.h"

BlendPass::BlendPass() {}

BlendPass::~BlendPass() {}

void BlendPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
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
}

void BlendPass::AddRenderPassDatas(std::string_view sceneName)
{
    Global::renderPassDatas->AddRenderPassProperty("ToneMappingPass", ToneMappingProperty({{1.f, 1.f, 1.f}, 1.f, 1.f, 1.f}));
}

void BlendPass::Begin(ID3D12GraphicsCommandList* commandList)
{    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    _finalRenderTarget->ClearRenderTarget(commandList);
    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, nullptr);

    _ownerScene->_accumulationBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void BlendPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    const auto& toneMappingProperty = std::any_cast<const ToneMappingProperty&>(Global::renderPassDatas->GetRenderPassProperty("ToneMappingPass"));

    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_6_tonMappingProperty"), 6, &toneMappingProperty, 0);
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("screenTexture"), _meshRenderTarget->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("sourceTexture"), _ownerScene->_accumulationBuffer->GetSRVHandle());

    _ownerScene->_frameQuad->Render(commandList);
}

void BlendPass::End(ID3D12GraphicsCommandList* commandList)
{    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}