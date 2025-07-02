#include "pch.h"
#include "RenderPass.h"
#include "RenderScene.h"

RenderPass::RenderPass()
    : _ownerScene(nullptr) {}

RenderPass::~RenderPass() {}

void RenderPass::SetOwnerScene(RenderScene* owner)
{
    _ownerScene = owner;
}

void RenderPass::Initialize()
{
    _meshRenderTarget  = UmMultiRenderTargetManager.GetRenderTarget(_ownerScene->_meshRenderTargetName);
    _finalRenderTarget = UmMultiRenderTargetManager.GetRenderTarget(_ownerScene->_finalTargetName);
}

void RenderPass::Begin(ID3D12GraphicsCommandList* commandList)
{
}

void RenderPass::End(ID3D12GraphicsCommandList* commandList)
{
}