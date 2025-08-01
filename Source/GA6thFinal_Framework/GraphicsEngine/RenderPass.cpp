#include "pch.h"
#include "RenderPass.h"

RenderPass::RenderPass() {}

RenderPass::~RenderPass() {}

void RenderPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    _ownerScene         = ownerScene;
    _ownerTechnique     = ownerTechnique;
    _sharedRenderTarget = ownerScene->GetSharedRenderTarget();

    _meshRenderTarget  = Global::multiRenderTargetManager->GetRenderTarget(_ownerScene->_meshRenderTargetName);
    _finalRenderTarget = Global::multiRenderTargetManager->GetRenderTarget(_ownerScene->_finalTargetName);
}