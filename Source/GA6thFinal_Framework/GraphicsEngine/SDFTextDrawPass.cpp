#include "pch.h"
#include "SDFTextDrawPass.h"
#include "UITechnique.h"
#include "SDFTextRenderer.h"
#include "SDFFont.h"
#include "FrameResource.h"

SDFTextDrawPass::SDFTextDrawPass(const std::vector<UINT>* instanceIDs)
    : UIPassBase(instanceIDs)
{
}

SDFTextDrawPass::~SDFTextDrawPass() = default;

void SDFTextDrawPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase::Initialize(ownerScene, ownerTechnique, commandList);

    PipelineStateStream pss;
    pss.BlendState                   = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.RasterizerState              = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode = D3D12_CULL_MODE_NONE;
    pss.DepthStencilState            = CD3DX12_DEPTH_STENCIL_DESC(CommonStates::DepthRead);
    pss.PrimitiveTopology            = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                   = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    pss.DSVFormat                    = _ownerScene->_depthStencilView->GetFormat();

    _fxSDF.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);

    _cameraBuffer = static_cast<UITechnique*>(_ownerTechnique)->GetCameraBuffer(MODE_2D);
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

    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, &_depthStencilView->GetDSVHandle());
    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void SDFTextDrawPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fxSDF.GetRootSignature());
    commandList->SetGraphicsRootConstantBufferView(_fxSDF.GetRootParameterIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());
    frameResource->SetFrameResource(FrameResourceType::TEXT_MATRICES, _fxSDF.GetRootParameterIndex("text_matrices"), commandList);

    SDFParams sdfParams = {};
    for (auto& component : _ownerScene->_sdfTextRenderQueue)
    {
        if (!component || !component->IsActive())
            continue;
        
        const SDFFont* font      = component->GetFont();
        const auto&    atlasInfo = font->GetAtlasInfo();
        const auto&    outline   = component->GetFontOutline();

        sdfParams.Flags         = component->GetFontFlags();
        sdfParams.DistanceRange = atlasInfo.DistanceRange;
        sdfParams.FontWeight    = component->GetFontWeight();
        sdfParams.OutlineColor  = outline.Color;
        sdfParams.OutlineWidth  = outline.Width;

        commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_4_fontColor"), 4, &component->GetColor(), 0);
        commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_9_sdfParams"), 9, &sdfParams, 0);
        commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("sdfTexture"), component->GetFontTextureHandle());

        component->Render(commandList);

        sdfParams.InstanceID++;
    }
}