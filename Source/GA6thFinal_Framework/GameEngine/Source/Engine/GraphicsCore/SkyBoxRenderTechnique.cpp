#include "pch.h"
#include "SkyBoxRenderTechnique.h"
#include "SkyBoxPass.h"
#include "RenderScene.h"

SkyBoxRenderTechnique::SkyBoxRenderTechnique() {}

SkyBoxRenderTechnique::~SkyBoxRenderTechnique() {}

void SkyBoxRenderTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
{
    InitSkyBoxPass();
}

void SkyBoxRenderTechnique::Execute(ID3D12GraphicsCommandList* commadList) 
{
    __super::Execute(commadList);
}

void SkyBoxRenderTechnique::InitSkyBoxPass() 
{
    std::shared_ptr<SkyBoxPass> pass = std::make_shared<SkyBoxPass>();
    pass->SetOwnerScene(_ownerScene);
    D3D12_VIEWPORT viewport{.TopLeftX = 0,
                            .TopLeftY = 0,
                            .Width    = (FLOAT)UmDevice.GetMode().Width,
                            .Height   = (FLOAT)UmDevice.GetMode().Height,
                            .MinDepth = 0.f,
                            .MaxDepth = 1.f};
    D3D12_RECT     scissor{
            .left = 0, .top = 0, .right = (LONG)UmDevice.GetMode().Width, .bottom = (LONG)UmDevice.GetMode().Height};
    pass->Initialize(viewport, scissor);
    AddRenderPass(pass);
}
