#include "pch.h"
#include "RenderPass.h"
#include "RenderScene.h"

RenderPass::RenderPass()
    : _shader(nullptr) 
{
}

RenderPass::~RenderPass() {}

void RenderPass::SetClearValue(const Color& clearColor, float depthClear, UINT clearStencil)
{
    _clearColor   = clearColor;
    _clearDepth   = depthClear;
    _clearStencil = clearStencil;
}

void RenderPass::SetOwnerScene(RenderScene* owner)
{
    _ownerScene = owner;
}

void RenderPass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    _viewPort   = viewPort;
    _sissorRect = sissorRect;

    _clearColor = Color(0.f, 0.f, 0.f, 1.f); 
    _clearDepth = 1.f; 
    _clearStencil = 0;

    _meshRenderTarget  = UmMultiRenderTargetManager.GetRenderTarget(_ownerScene->_meshRenderTargetName);
    _finalRenderTarget = UmMultiRenderTargetManager.GetRenderTarget(_ownerScene->_finalTargetName);
}

void RenderPass::Begin(ID3D12GraphicsCommandList* commandList)
{
}

void RenderPass::End(ID3D12GraphicsCommandList* commandList)
{
}