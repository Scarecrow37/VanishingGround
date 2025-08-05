#include "pch.h"
#include "SkyBoxRenderTechnique.h"
#include "SkyBoxPass.h"

SkyBoxRenderTechnique::SkyBoxRenderTechnique() {}

SkyBoxRenderTechnique::~SkyBoxRenderTechnique() {}

void SkyBoxRenderTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;

    pass = std::make_unique<SkyBoxPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void SkyBoxRenderTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}