#include "pch.h"
#include "ParticleSpritePass.h"

ParticleSpritePass::ParticleSpritePass() = default;

ParticleSpritePass::~ParticleSpritePass() = default;

void ParticleSpritePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitializeShaderAndPSO();
    _textureIDBuffer = std::make_unique<StructuredBuffer>();
    _textureIDBuffer->Initialize(sizeof(int), 100);
}

void ParticleSpritePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto customDepthTarget  = Global::multiRenderTargetManager->GetRenderTarget("CustomDepth");
    customDepthTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

   // _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_READ);

    _accumulateBuffer->ClearUnorderedAccessView(commandList, Vector4(0.f, 0.f, 0.f, 0.f));
    _revealageBuffer->ClearUnorderedAccessView(commandList, Vector4(0.f, 0.f, 0.f, 0.f));

    commandList->OMSetRenderTargets(1, &customDepthTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &customDepthTarget->GetViewport());
    commandList->RSSetScissorRects(1, &customDepthTarget->GetScissorRect());

    ComPtr<ID3D12Resource> resource = Global::particleManager->GetComputeOutputResource(_ownerScene->_name);

    auto computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    commandList->ResourceBarrier(1, &computeOutputBarrior);

    _albedoTextureIDs = Global::particleManager->GetActiveAlbedos(_ownerScene->_name);
    _textureIDBuffer->CopyStructuredBuffer(commandList, _albedoTextureIDs.data(),
                                           static_cast<UINT>(_albedoTextureIDs.size()));
}

void ParticleSpritePass::Draw(ID3D12GraphicsCommandList* commandList)
{        
    if (0 >= Global::particleManager->GetTotalCount(_ownerScene->_name))
        return;

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    auto depthStencilBuffer = Global::multiRenderTargetManager->GetRenderTarget("Depth");

    auto            customDepthTarget = Global::multiRenderTargetManager->GetRenderTarget("CustomDepth");
    const auto&     resolution        = customDepthTarget->GetResolution();
    PostProcessData postProcessData{.TexelSize = {1.f / (float)resolution.cx, 1.f / (float)resolution.cy}};
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_6_postProcessData"), 6,
                                               &postProcessData, 0);

    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("depthbuffer"),
                                                depthStencilBuffer->GetSRVHandle());

    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("texID"),
                                                   _textureIDBuffer->GetGPUVirtualAddress());

    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("gAccumTex"),
                                                _accumulateBuffer->GetUAVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("gRevealTex"),
                                                _revealageBuffer->GetUAVHandle());

    auto outputResource = Global::particleManager->GetComputeOutputResource(_ownerScene->_name);
    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("particleInfo"),
                                                   outputResource->GetGPUVirtualAddress());

    D3D12_GPU_DESCRIPTOR_HANDLE descHeapPtr =
        Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();

    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("textures"), descHeapPtr);

    // 그래픽스 큐는 해당 Fence값에 도달할 때() 까지 대기
    // (일단 이렇게 하는데 이건 해당 컴퓨트 큐의 영향을 받는 명령어 직전에 호출해주는게 제일 좋음)
    UINT64 fence = Global::particleManager->GetComputeFenceValue(_ownerScene->_name);
    Global::commandController->WaitCommandQueue(GRAPHICS_QUEUE, COMPUTE_QUEUE, fence);

    UINT totalCount = Global::particleManager->GetTotalCount(_ownerScene->_name);
    _ownerScene->_frameQuad->Render(commandList, totalCount);
}

void ParticleSpritePass::End(ID3D12GraphicsCommandList* commandList)
{
    ComPtr<ID3D12Resource> resource             = Global::particleManager->GetComputeOutputResource(_ownerScene->_name);
    auto                   computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);

    commandList->ResourceBarrier(1, &computeOutputBarrior);

    _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_PRESENT);
    _accumulateBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    _revealageBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ParticleSpritePass::InitializeShaderAndPSO()
{
    PipelineStateStream pss;
    pss.RasterizerState                   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    (&pss.RasterizerState)->CullMode      = D3D12_CULL_MODE_NONE;
    pss.BlendState                        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    pss.DepthStencilState                 = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    (&pss.DepthStencilState)->DepthEnable = FALSE;
    pss.PrimitiveTopology                 = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pss.RTVFormats                        = {{DXGI_FORMAT_R32_UINT}, 1};
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void ParticleSpritePass::SetAccumulationBuffers(SharedResource<UnorderedAccessView> color, SharedResource<UnorderedAccessView> alpha)
{
    _accumulateBuffer = color;
    _revealageBuffer = alpha;
}