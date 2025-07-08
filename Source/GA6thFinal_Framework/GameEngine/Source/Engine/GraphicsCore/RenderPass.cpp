#include "pch.h"
#include "RenderPass.h"

RenderPass::RenderPass() {}

RenderPass::~RenderPass() {}

void RenderPass::Initialize(RenderScene* ownerScene)
{
    _ownerScene     = ownerScene;

    _meshRenderTarget  = UmMultiRenderTargetManager.GetRenderTarget(_ownerScene->_meshRenderTargetName);   
    _finalRenderTarget = UmMultiRenderTargetManager.GetRenderTarget(_ownerScene->_finalTargetName);
}