#include "pch.h"
#include "LightingTechnique.h"
#include "DeferredPBRLitPass.h"
#include "ShadowMapPass.h"
#include "PointLightShadowPass.h"
#include "GBufferPass.h"
#include "SSAOWritePass.h"
#include "ForwardPBRLitPass.h"
#include "CustomShaderPass.h"

LightingTechnique::LightingTechnique() = default;

LightingTechnique::~LightingTechnique() = default;

void LightingTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
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

    /*pass = std::make_unique<ForwardPBRLitPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));*/

    pass = std::make_unique<CustomShaderPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}