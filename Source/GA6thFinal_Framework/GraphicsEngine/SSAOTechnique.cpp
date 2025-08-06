#include "pch.h"
#include "SSAOTechnique.h"
#include "SSAOWritePass.h"
#include "SSAOBlendPass.h"

SSAOTechnique::SSAOTechnique() {}

SSAOTechnique::~SSAOTechnique() {}

void SSAOTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
{
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<SSAOWritePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
    pass = std::make_unique<SSAOBlendPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void SSAOTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    __super::Execute(commandList);
}
