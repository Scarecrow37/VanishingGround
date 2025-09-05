#include "pch.h"
#include "FadePass.h"

FadePass::FadePass() {}

FadePass::~FadePass() {}

void FadePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                          ID3D12GraphicsCommandList* commandList)
{

}


void FadePass::Update(ID3D12GraphicsCommandList* commadList)
{
    if (true == _fadeFlag)
    {
        _fadeTimer += Global::renderer->GetDeltaTime();
        if (_fadeTimer >= _fadeDuration)
            _fadeFlag = false;
    }
    
}

void FadePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    if (false == _fadeFlag)
        return;
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);


    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void FadePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    if (false == _fadeFlag)
        return;

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    //test
    timestep t = {_fadeTimer / _fadeDuration, _fadeStart, _fadeEnd};
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_9_time"), 9, &t, 0);
    _ownerScene->_frameQuad->Render(commandList);
}

void FadePass::End(ID3D12GraphicsCommandList* commandList)
{
    if (false == _fadeFlag)
        return;

    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}



void FadePass::Fade(Vector4& start, Vector4& end, float time) 
{
    if (true == _fadeFlag)
        return;
    _fadeStart = start;
    _fadeEnd   = end;
    _fadeDuration = time;
    _fadeTimer    = 0;
    _fadeFlag     = true;
}

void FadePass::InitializeShaderAndPSO() 
{
    D3D12_BLEND_DESC blendDesc       = {};
    blendDesc.AlphaToCoverageEnable  = FALSE;
    blendDesc.IndependentBlendEnable = TRUE;
    auto& rtDesc                     = blendDesc.RenderTarget[0];
    rtDesc.BlendEnable               = TRUE;
    rtDesc.SrcBlend                  = D3D12_BLEND_SRC_ALPHA;
    rtDesc.DestBlend                 = D3D12_BLEND_INV_SRC_ALPHA;
    rtDesc.BlendOp                   = D3D12_BLEND_OP_ADD;
    rtDesc.SrcBlendAlpha             = D3D12_BLEND_ONE;
    rtDesc.DestBlendAlpha            = D3D12_BLEND_ZERO;
    rtDesc.BlendOpAlpha              = D3D12_BLEND_OP_ADD;
    rtDesc.RenderTargetWriteMask     = D3D12_COLOR_WRITE_ENABLE_ALL;

    PipelineStateStream pss;
    pss.RasterizerState                       = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode          = D3D12_CULL_MODE_BACK;
    (&pss.BlendState)->AlphaToCoverageEnable  = FALSE;
    (&pss.BlendState)->IndependentBlendEnable = TRUE;
    (&pss.BlendState)->RenderTarget[0]        = rtDesc;
    pss.DepthStencilState                     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable     = FALSE;
    pss.PrimitiveTopology                     = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                            = {{DXGI_FORMAT_R32G32B32A32_FLOAT}, 1};
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}
