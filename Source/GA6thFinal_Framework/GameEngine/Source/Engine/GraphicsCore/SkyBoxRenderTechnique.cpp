#include "pch.h"
#include "SkyBoxRenderTechnique.h"
#include "SkyBoxPass.h"

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
    std::unique_ptr<SkyBoxPass> pass = std::make_unique<SkyBoxPass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));
}
