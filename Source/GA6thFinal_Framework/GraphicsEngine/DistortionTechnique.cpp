#include "pch.h"
#include "DistortionResolvePass.h"
#include "DistortionTechnique.h"

DistortionTechnique::DistortionTechnique() {}

DistortionTechnique::~DistortionTechnique() {}

void DistortionTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
{
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<DistortionResolvePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));


}

void DistortionTechnique::Execute(ID3D12GraphicsCommandList* commandList) {}
