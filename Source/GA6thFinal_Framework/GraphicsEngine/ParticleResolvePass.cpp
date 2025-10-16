#include "pch.h"
#include "ParticleResolvePass.h"

ParticleResolvePass::ParticleResolvePass() = default;

ParticleResolvePass::~ParticleResolvePass() = default;

void ParticleResolvePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitializeShaderAndPSO();
}

void ParticleResolvePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->OMSetRenderTargets(1, &_meshRenderTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_meshRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_meshRenderTarget->GetScissorRect());
}

void ParticleResolvePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());


    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("gAccumTex"), _accumulateBuffer->GetSRVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("gRevealTex"), _revealageBuffer->GetSRVHandle());

    _accumulateBuffer->ResourceBarrier(commandList);
    _revealageBuffer->ResourceBarrier(commandList);

    _ownerScene->_frameQuad->Render(commandList);
}

void ParticleResolvePass::End(ID3D12GraphicsCommandList* commandList)
{
    _accumulateBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _revealageBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _meshRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ParticleResolvePass::SetAccumulationBuffers(SharedResource<UnorderedAccessView> color, SharedResource<UnorderedAccessView> alpha)
{
    _accumulateBuffer = color;
    _revealageBuffer = alpha;
}


void ParticleResolvePass::InitializeShaderAndPSO() 
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
    (&pss.RasterizerState)->CullMode          = D3D12_CULL_MODE_NONE;
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


