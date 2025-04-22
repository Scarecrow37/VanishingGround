#include "pch.h"
#include "RenderPass.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "Shader.h"
RenderPass::RenderPass() : _clearColor{Color(0.f, 0.f, 0.f, 1.f)}, _clearDepth{1.f}, _clearStencil{0} {}

RenderPass::~RenderPass() {}

void RenderPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    _viewPort   = viewPort;
    _sissorRect = sissorRect;
}

void RenderPass::SetDescriptors(const DescriptorSet& descriptors)
{
    _descriptor = descriptors;
}

void RenderPass::SetClearValue(const Color& clearColor, float depthClear, UINT clearStencil)
{
    _clearColor   = clearColor;
    _clearDepth   = depthClear;
    _clearStencil = clearStencil;
}

void RenderPass::SetShader(std::shared_ptr<Shader> shader)
{
    _shader = shader;
}

void RenderPass::SetPipelineState(ComPtr<ID3D12PipelineState> pso)
{
    _pipelineState = pso;
}

void RenderPass::SetOwnerScene(RenderScene* owner)
{
    _ownerScene = owner;
}

void RenderPass::Begin(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    CD3DX12_RESOURCE_BARRIER barrier;
    for (UINT i = 0; i < _descriptor.RenderTagetCount; ++i)
    {
        if (_descriptor.RenderTargetResources[i])
        {
            barrier =
                CD3DX12_RESOURCE_BARRIER::Transition(_descriptor.RenderTargetResources[i].Get(),
                                                     D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandList->ResourceBarrier(1, &barrier);
            commandList->ClearRenderTargetView(_descriptor.RenderTargetViewsHandle[i], _clearColor, 0, nullptr);
        }
    }

    if (_descriptor.UseDepthStencilView && _descriptor.DepthStencilResource)
    {
        barrier = CD3DX12_RESOURCE_BARRIER::Transition(_descriptor.DepthStencilResource.Get(),
                                                       D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        commandList->ResourceBarrier(1, &barrier);
    }

    if (_descriptor.UseDepthStencilView)
    {
        commandList->ClearDepthStencilView(_descriptor.DepthStencilViewHandle,
                                           D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, _clearDepth,
                                           _clearStencil, 0, nullptr);
    }
    commandList->OMSetRenderTargets(_descriptor.RenderTagetCount, _descriptor.RenderTargetViewsHandle.data(), FALSE,
                                    _descriptor.UseDepthStencilView ? &_descriptor.DepthStencilViewHandle : nullptr);
    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);
    if (_shader)
        commandList->SetGraphicsRootSignature(_shader->GetRootSignature().Get());

    if (_pipelineState)
        commandList->SetPipelineState(_pipelineState.Get());
}

void RenderPass::End(ComPtr<ID3D12GraphicsCommandList> commandList)
{
    CD3DX12_RESOURCE_BARRIER barrier;
    for (UINT i = 0; i < _descriptor.RenderTagetCount; ++i)
    {
        if (_descriptor.RenderTargetResources[i])
        {
            barrier =
                CD3DX12_RESOURCE_BARRIER::Transition(_descriptor.RenderTargetResources[i].Get(),
                                                           D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                           D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
            commandList->ResourceBarrier(1, &barrier);
        }
    }

    if (_descriptor.UseDepthStencilView && _descriptor.DepthStencilResource)
    {
        barrier = CD3DX12_RESOURCE_BARRIER::Transition(_descriptor.DepthStencilResource.Get(),
                                                       D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PRESENT);
        commandList->ResourceBarrier(1, &barrier);
    }
}

void RenderPass::DescriptorSet::CreateDescriptorSet(UINT                        renderTargetCount,
                                                    D3D12_CPU_DESCRIPTOR_HANDLE depthstencilHandle,
                                                    ComPtr<ID3D12Resource> depthStencilBuffer, RenderScene* ownerScene,
                                                    bool UseDepth)
{
    RenderTagetCount = renderTargetCount;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> rtCpuHandles;
    std::vector<ComPtr<ID3D12Resource>>      rtResources;
    rtCpuHandles.resize(renderTargetCount);
    rtResources.resize(renderTargetCount);
    // 특히 여기. 이래 rt pool 돌려씀...
    for (int i = 0; i < renderTargetCount; ++i)
    {
        rtCpuHandles[i] = ownerScene->_renderTargetHandles[ownerScene->_currnetRederTarget];
        rtResources[i]  = ownerScene->_renderTargetPool[ownerScene->_currnetRederTarget]->GetResource();
        ownerScene->_currnetRederTarget++;
        if (ownerScene->_currnetRederTarget >= ownerScene->_renderTargetPool.size())
        {
            ownerScene->_currnetRederTarget -= ownerScene->_renderTargetPool.size();
        }
    }
    RenderTargetViewsHandle = rtCpuHandles;
    RenderTargetResources   = rtResources;
    DepthStencilViewHandle  = depthstencilHandle;
    DepthStencilResource    = depthStencilBuffer;
    UseDepthStencilView     = UseDepth;
}