#include "pch.h"
#include "SkyBoxRenderTechnique.h"
#include "SkyBoxPass.h"

SkyBoxRenderTechnique::SkyBoxRenderTechnique() = default;

SkyBoxRenderTechnique::~SkyBoxRenderTechnique() = default;

void SkyBoxRenderTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;

    pass = std::make_unique<SkyBoxPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}