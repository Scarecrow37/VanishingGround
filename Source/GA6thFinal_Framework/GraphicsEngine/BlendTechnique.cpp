#include "pch.h"
#include "BlendTechnique.h"
#include "BlendPass.h"

BlendTechnique::BlendTechnique() = default;

BlendTechnique::~BlendTechnique() = default;

void BlendTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;    
    pass = std::make_unique<BlendPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}