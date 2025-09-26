#include "pch.h"
#include "SDFTextDrawPass.h"
#include "UITechnique_OIT.h"
#include "SDFTextRenderer.h"

SDFTextDrawPass::SDFTextDrawPass(const std::vector<UINT>* instanceIDs)
    : UIPassBase_OIT(instanceIDs)
{
}

SDFTextDrawPass::~SDFTextDrawPass() = default;

void SDFTextDrawPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase_OIT::Initialize(ownerScene, ownerTechnique, commandList);

    PipelineStateStream pss;
    pss.BlendState                   = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState              = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    pss.DepthStencilState            = CD3DX12_DEPTH_STENCIL_DESC(CommonStates::DepthRead);
    pss.PrimitiveTopology            = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.DSVFormat                    = _ownerScene->_depthStencilView->GetFormat();

    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);

    _depthStencilView = static_cast<UITechnique_OIT*>(_ownerTechnique)->GetDepthStencilView();
    _cameraBuffer     = static_cast<UITechnique_OIT*>(_ownerTechnique)->GetCameraBuffer(MODE_2D);
}

void SDFTextDrawPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_READ);

    commandList->OMSetRenderTargets(0, nullptr, FALSE, &_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void SDFTextDrawPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("OITHead"), _headBuffer->GetUAVHandle());
    commandList->SetGraphicsRootUnorderedAccessView(_fx.GetRootParameterIndex("OITNodes"), _nodesBuffer->GetGPUVirtualAddress());  
    commandList->SetGraphicsRootUnorderedAccessView(_fx.GetRootParameterIndex("OITCounter"), _atomicCounterBuffer->GetGPUVirtualAddress());

    UINT index = 0;
    for (auto& [isDestroy, component] : _ownerScene->_sdfTextRenderQueue)
    {
        if (!component || !component->IsActive())
            continue;
        
        commandList->SetComputeRoot32BitConstants(_fx.GetRootParameterIndex("bit32_1_instanceID"), 1, &(*_instanceIDs)[index++], 0);
        commandList->SetComputeRoot32BitConstants(_fx.GetRootParameterIndex("bit32_4_fontColor"), 4, &component->GetColor(), 0);
        commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("sdfTexture"), component->GetFontTextureHandle());

        component->Render(commandList);
    }
}

void SDFTextDrawPass::End(ID3D12GraphicsCommandList* commandList)
{
}
