#include "pch.h"
#include "SSRTechnique.h"
#include "SSRPass.h"

SSRTechnique::SSRTechnique() {}

SSRTechnique::~SSRTechnique() {}

void SSRTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<SSRPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void SSRTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    __super::Execute(commandList);
}
