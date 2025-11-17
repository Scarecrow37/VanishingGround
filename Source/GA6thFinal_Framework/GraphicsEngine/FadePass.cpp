#include "pch.h"
#include "FadePass.h"

FadePass::FadePass() = default;

FadePass::~FadePass() = default;

void FadePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                          ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitializeShaderAndPSO();
}

void FadePass::Update(ID3D12GraphicsCommandList* commadList, const float deltaTime)
{
    auto& props = Global::sceneTransitionCore->_sceneTransitionProps;
    auto  it    = props.find(_ownerScene->_name);
    if (it != props.end())
    {
        auto& transitionProp = it->second;
        if (true == transitionProp._fadeFlag)
        {
            _fadeFlag  = true;
            _fadeColor = transitionProp._fadeColor;
        }
        else
        {
            _fadeFlag = false;
        }
    }
}

void FadePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    if (false == _fadeFlag)
    {
        return;
    }
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void FadePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    if (false == _fadeFlag)
    {
        return;
    }
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_4_time"), 4, &_fadeColor, 0);
    _ownerScene->_frameQuad->Render(commandList);
}

void FadePass::End(ID3D12GraphicsCommandList* commandList)
{
    if (false == _fadeFlag)
    {
        return;
    }
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void FadePass::InitializeShaderAndPSO()
{
    PipelineStateStream pss;
    pss.RasterizerState                       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode          = D3D12_CULL_MODE_NONE;
    pss.BlendState                            = CD3DX12_BLEND_DESC(CommonStates::NonPremultiplied);
    pss.DepthStencilState                     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable     = FALSE;
    pss.PrimitiveTopology                     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                            = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}
