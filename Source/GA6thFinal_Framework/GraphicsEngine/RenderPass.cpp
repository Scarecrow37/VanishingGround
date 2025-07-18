#include "pch.h"
#include "RenderPass.h"

RenderPass::RenderPass() {}

RenderPass::~RenderPass() {}

void RenderPass::Initialize(RenderScene* ownerScene)
{
    _ownerScene     = ownerScene;

    _meshRenderTarget  = Global::multiRenderTargetManager->GetRenderTarget(_ownerScene->_meshRenderTargetName);
    _finalRenderTarget = Global::multiRenderTargetManager->GetRenderTarget(_ownerScene->_finalTargetName);
}