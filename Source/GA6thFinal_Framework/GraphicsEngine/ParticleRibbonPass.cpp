#include "pch.h"
#include "ParticleRibbonPass.h"

ParticleRibbonPass::ParticleRibbonPass() {}

ParticleRibbonPass::~ParticleRibbonPass() {}

void ParticleRibbonPass::SetAccumulationBuffers(SharedResource<UnorderedAccessView> color,
                                                SharedResource<UnorderedAccessView> alpha)
{
    _accumlateBuffer = color;
    _revealageBuffer = alpha;
}

void ParticleRibbonPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);

    InitializeShaderAndPSO();

    // refactoring needed
    _albedoTextureIDs = std::vector<int>(100, -1);

    _textureIDBuffer = std::make_unique<StructuredBuffer>();
    _textureIDBuffer->Initialize(sizeof(int), 100);
    _ribbonIndexBuffer.resize(100);
    _ribbonIndices.resize(100);

    for (int i = 0; i < 100;i++)
    {
        _ribbonIndexBuffer[i] = std::make_unique<StructuredBuffer>();
        _ribbonIndexBuffer[i]->Initialize(sizeof(UINT), 100000);
    }
}

void ParticleRibbonPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    auto customDepthTarget = Global::multiRenderTargetManager->GetRenderTarget("CustomDepth");
    customDepthTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);

    commandList->OMSetRenderTargets(1, &customDepthTarget->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &customDepthTarget->GetViewport());
    commandList->RSSetScissorRects(1, &customDepthTarget->GetScissorRect());

    ComPtr<ID3D12Resource> resource = Global::particleManager->GetComputeOutputResource(_ownerScene->_name);

    auto computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                                     D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

    commandList->ResourceBarrier(1, &computeOutputBarrior);

    auto albedoTextures = Global::particleManager->GetActiveRibbonAlbedos(_ownerScene->_name);
    std::fill(_albedoTextureIDs.begin(), _albedoTextureIDs.end(), -1);
    for (int i = 0; i < albedoTextures.size(); ++i)
    {
        _albedoTextureIDs[i] = albedoTextures[i]->GetID();
    }

    _textureIDBuffer->CopyStructuredBuffer(commandList, _albedoTextureIDs.data(),
                                           static_cast<UINT>(albedoTextures.size()));

    if (0 < Global::particleManager->GetRibbonCount(_ownerScene->_name))
    {
        _ribbonIndices.clear();
        auto totalribbonemitterindices = Global::particleManager->GetRibbonEmitterIndices(_ownerScene->_name);
        _ribbonIndices.resize(totalribbonemitterindices.size());
        for (int i = 0; i < totalribbonemitterindices.size(); i++)
        {
            std::sort(totalribbonemitterindices[i].begin(), totalribbonemitterindices[i].end(),
                      [](const RibbonIndex& a, const RibbonIndex& b) -> bool { return a.ratio < b.ratio; });

            auto size = totalribbonemitterindices[i].size();
            _ribbonIndices[i].resize(size);
            std::fill(_ribbonIndices[i].begin(), _ribbonIndices[i].end(), -1);
            for (int j = 0; j < size; j++)
            {
                _ribbonIndices[i][j] = totalribbonemitterindices[i][j].index;
            }
            _ribbonIndexBuffer[i]->CopyStructuredBuffer(commandList, _ribbonIndices[i].data(),
                                                        static_cast<UINT>(totalribbonemitterindices[i].size()));
        }
    }
}

void ParticleRibbonPass::Draw(ID3D12GraphicsCommandList* commandList)
{

    if (0 >= Global::particleManager->GetRibbonCount(_ownerScene->_name))
        return;
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());
    auto depthStencilBuffer = Global::multiRenderTargetManager->GetRenderTarget("Depth");

    auto        customDepthTarget = Global::multiRenderTargetManager->GetRenderTarget("CustomDepth");
    const auto& mode              = customDepthTarget->GetResolution();

    PostProcessData postProcessData{.TexelSize = {1.f / (float)mode.Width, 1.f / (float)mode.Height}};
    commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_6_postProcessData"), 6,
                                               &postProcessData, 0);

    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("depthbuffer"),
                                                depthStencilBuffer->GetSRVHandle());

    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"),
                                                   _ownerScene->_cameraBuffer->GetGPUVirtualAddress());

    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("texID"),
                                                   _textureIDBuffer->GetGPUVirtualAddress());

    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("gAccumTex"),
                                                _accumlateBuffer->GetUAVHandle());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("gRevealTex"),
                                                _revealageBuffer->GetUAVHandle());

    auto outputResource = Global::particleManager->GetRibbonOutputResource(_ownerScene->_name);
    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("particleInfo"),
                                                   outputResource->GetGPUVirtualAddress());

    D3D12_GPU_DESCRIPTOR_HANDLE descHeapPtr =
        Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();

    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("textures"), descHeapPtr);

    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    D3D12_VERTEX_BUFFER_VIEW nullView{};
    commandList->IASetVertexBuffers(0, 1, &nullView);

    for (int i = 0; i < _ribbonIndices.size(); ++i)
    {
        UINT ribbonSegmentCount = static_cast<UINT>(_ribbonIndices[i].size());
        if (2 >= ribbonSegmentCount)
            continue;

        UINT vertexCount = (ribbonSegmentCount - 1) * 2;
        if (vertexCount == 0)
            continue;
        commandList->SetGraphicsRoot32BitConstants(_fx.GetRootParameterIndex("bit32_1_ribbonVertexCount"), 1, &vertexCount, 0);

        commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("ribbonIndices"),
                                                       _ribbonIndexBuffer[i]->GetGPUVirtualAddress());
        commandList->DrawInstanced(vertexCount, 1, 0, 0);
    }
}

void ParticleRibbonPass::End(ID3D12GraphicsCommandList* commandList) 
{
    ComPtr<ID3D12Resource> resource             = Global::particleManager->GetComputeOutputResource(_ownerScene->_name);
    auto                   computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COMMON);

    commandList->ResourceBarrier(1, &computeOutputBarrior);

    _ownerScene->_depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_PRESENT);
    _accumlateBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    _revealageBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void ParticleRibbonPass::InitializeShaderAndPSO() 
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
