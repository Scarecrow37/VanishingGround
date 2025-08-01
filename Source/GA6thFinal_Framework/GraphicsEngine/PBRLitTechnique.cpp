#include "pch.h"
#include "PBRLitTechnique.h"
#include "DeferredPBRLitPass.h"
#include "ShadowMapPass.h"
#include "GBufferPass.h"

PBRLitTechnique::PBRLitTechnique() {}

PBRLitTechnique::~PBRLitTechnique() {}

void PBRLitTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;

    pass = std::make_unique<GBufferPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<ShadowMapPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<DeferredPBRLitPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void PBRLitTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}