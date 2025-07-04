#include "pch.h"
#include "BlendTechnique.h"
#include "BlendPass.h"

BlendTechnique::BlendTechnique() {}

BlendTechnique::~BlendTechnique() {}

void BlendTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;    
    pass = std::make_unique<BlendPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));
}

void BlendTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}