#include "pch.h"
#include "BloomTechnique.h"
#include "BrightExtractPass.h"
#include "DownAndUpSamplingPass.h"

BloomTechnique::BloomTechnique() {}

BloomTechnique::~BloomTechnique() {}

void BloomTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{    
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<BrightExtractPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<DownAndUpSamplingPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void BloomTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}