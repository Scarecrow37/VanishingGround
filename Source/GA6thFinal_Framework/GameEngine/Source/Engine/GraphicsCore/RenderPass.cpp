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

void RenderPass::SetClearValue(const Color& clearColor, float depthClear, UINT clearStencil)
{
    _clearColor   = clearColor;
    _clearDepth   = depthClear;
    _clearStencil = clearStencil;
}

void RenderPass::SetShader(std::shared_ptr<ShaderBuilder> shader)
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

void RenderPass::Begin(ID3D12GraphicsCommandList* commandList)
{
}

void RenderPass::End(ID3D12GraphicsCommandList* commandList)
{
}