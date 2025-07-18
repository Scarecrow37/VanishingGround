#include "pch.h"
#include "SkyBoxRenderTechnique.h"
#include "SkyBoxPass.h"

SkyBoxRenderTechnique::SkyBoxRenderTechnique() {}

SkyBoxRenderTechnique::~SkyBoxRenderTechnique() {}

void SkyBoxRenderTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
{
    std::unique_ptr<SkyBoxPass> pass = std::make_unique<SkyBoxPass>();
    pass->Initialize(_ownerScene, commandList);
    AddRenderPass(std::move(pass));
}

void SkyBoxRenderTechnique::Execute(ID3D12GraphicsCommandList* commadList) 
{
    __super::Execute(commadList);
}