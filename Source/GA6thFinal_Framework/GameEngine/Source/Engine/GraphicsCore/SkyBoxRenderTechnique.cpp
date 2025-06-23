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
    const auto&    mode = UmDevice.GetMode();
    D3D12_VIEWPORT viewport{.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    D3D12_RECT     scissor{.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};

    std::unique_ptr<SkyBoxPass> pass = std::make_unique<SkyBoxPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));
}
