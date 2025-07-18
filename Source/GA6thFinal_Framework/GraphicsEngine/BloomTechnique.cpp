#include "pch.h"
#include "BloomTechnique.h"
#include "BrightExtractPass.h"
#include "DownScalePass.h"
#include "UpScalePass.h"
#include "BlurXPass.h"
#include "BlurYPass.h"

BloomTechnique::BloomTechnique() {}

BloomTechnique::~BloomTechnique() {}

void BloomTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{    
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<BrightExtractPass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<DownScalePass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<UpScalePass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<BlurXPass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<BlurYPass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));
}

void BloomTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}