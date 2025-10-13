#include "pch.h"
#include "SDFTextDrawPass.h"
#include "UITechnique_OIT.h"
#include "SDFTextRenderer.h"
#include "SDFFont.h"
#include "FrameResource.h"

struct SDFParams
{
    unsigned int InstanceID;
    float        DistanceRange;
    float        FontWeight;
};

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

void SDFTextDrawPass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime)
{
    for (auto& component : _ownerScene->_sdfTextRenderQueue)
    {
        if (!component || !component->IsActive())
            continue;       

        component->Update(commandList);
    }
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
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("OITHead"), _headBuffer->GetUAVHandle());
    commandList->SetGraphicsRootUnorderedAccessView(_fx.GetRootParameterIndex("OITNodes"), _nodesBuffer->GetGPUVirtualAddress());  
    commandList->SetGraphicsRootUnorderedAccessView(_fx.GetRootParameterIndex("OITCounter"), _atomicCounterBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());
    frameResource->SetFrameResource(FrameResourceType::TEXT_MATRICES, _fx.GetRootParameterIndex("text_matrices"), commandList);

    SDFParams sdfParams = {};
    for (auto& component : _ownerScene->_sdfTextRenderQueue)
    {
        if (!component || !component->IsActive())
            continue;
        
        const SDFFont* font       = component->GetFont();
        const auto&    atlasInfo  = font->GetAtlasInfo();
        sdfParams.DistanceRange   = atlasInfo.DistanceRange;
        sdfParams.FontWeight      = component->GetFontWeight();

        commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_4_fontColor"), 4, &component->GetColor(), 0);
        commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_3_sdfParams"), 3, &sdfParams, 0);
        commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("sdfTexture"), component->GetFontTextureHandle());

        component->Render(commandList);

        sdfParams.InstanceID++;
    }
}