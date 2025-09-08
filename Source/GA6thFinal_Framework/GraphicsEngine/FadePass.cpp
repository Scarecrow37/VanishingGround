#include "pch.h"
#include "FadePass.h"

FadePass::FadePass() {}

FadePass::~FadePass() {}

void FadePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique,
                          ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
    InitializeShaderAndPSO();
}

void FadePass::Update(ID3D12GraphicsCommandList* commadList)
{
    // if (UmTransition._sceneTransitionProps[_ownerScene->_name])
    SceneTransitionProperty* transitionProp = Global::sceneTransitionCore->_sceneTransitionProps[_ownerScene->_name];
    if (nullptr != transitionProp && true == transitionProp->_fadeFlag)
    {
        _fadeFlag = true;
        _fadeStart = transitionProp->_fadeStartColor;
        _fadeEnd   = transitionProp->_fadeEndColor;
        _fadeDuration = transitionProp->_fadeDuration;
        _fadeTimer    = 0;
        Global::sceneTransitionCore->_sceneTransitionProps[_ownerScene->_name]->_fadeFlag = false;
    }

    if (true == _fadeFlag)
    {
        _fadeTimer += Global::renderer->GetDeltaTime();
        if (_fadeTimer >= _fadeDuration)
        {
            _fadeFlag = false;
            _fadeTimer = 0;
        }
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

    // test
    float    ratio = _fadeTimer / _fadeDuration;
    timestep t     = {_fadeStart,_fadeEnd, ratio};
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_9_time"), 9, &t, 0);
    _ownerScene->_frameQuad->Render(commandList);
}

void FadePass::End(ID3D12GraphicsCommandList* commandList)
{
    if (false == _fadeFlag)
        return;

    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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
    rtDesc.DestBlendAlpha            = D3D12_BLEND_ONE;
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
