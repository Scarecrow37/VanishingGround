#include "pch.h"
#include "PBRLitTechnique.h"
#include "DeferredPBRLitPass.h"
#include "ShadowMapPass.h"
#include "PointLightShadowPass.h"
#include "GBufferPass.h"
#include "SSAOWritePass.h"
#include "ForwardPBRLitPass.h"

PBRLitTechnique::PBRLitTechnique() = default;

PBRLitTechnique::~PBRLitTechnique() = default;

void PBRLitTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;

    pass = std::make_unique<GBufferPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<SSAOWritePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<ShadowMapPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<PointLightShadowPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<DeferredPBRLitPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<ForwardPBRLitPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}