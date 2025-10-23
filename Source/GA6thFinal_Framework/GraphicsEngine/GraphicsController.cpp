#include "pch.h"
#include "GraphicsController.h"

// render framework
#include "RenderScene.h"
#include "RenderTechnique.h"
#include "RenderPass.h"

// render technique
#include "SSRTechnique.h"
#include "LightingTechnique.h"
#include "VolumetricFogTechnique.h"
#include "BloomTechnique.h"

// render pass
#include "SSAOWritePass.h"
#include "GBufferPass.h"

GraphicsController::~GraphicsController() {}

void GraphicsController::SetSSR(std::string_view sceneName, bool enable)
{
    RenderScene* renderScene = Global::renderer->GetRenderScene(sceneName);
    auto ssrTech = renderScene->GetRenderTechnique<SSRTechnique>();
    if (ssrTech)
       ssrTech->SetEnable(enable);
}

void GraphicsController::SetSSAO(std::string_view sceneName, bool enable)
{
    RenderScene* renderScene = Global::renderer->GetRenderScene(sceneName);
    auto         pbrTech     = renderScene->GetRenderTechnique<LightingTechnique>();
    if (pbrTech)
    {
        auto ssaoPass = pbrTech->GetRenderPass<SSAOWritePass>();
        ssaoPass->SetEnable(enable);
    }
}

void GraphicsController::SetVolumetricFog(std::string_view sceneName, bool enable)
{
    RenderScene* renderScene = Global::renderer->GetRenderScene(sceneName);
    auto         volumeTech  = renderScene->GetRenderTechnique<VolumetricFogTechnique>();
    if (volumeTech)
        volumeTech->SetEnable(enable);
}

void GraphicsController::SetBloom(std::string_view sceneName, bool enable)
{
    RenderScene* renderScene = Global::renderer->GetRenderScene(sceneName);
    auto         bloomTech   = renderScene->GetRenderTechnique<BloomTechnique>();
    if (bloomTech)
        bloomTech->SetEnable(enable);
}

void GraphicsController::SetTextureQuality(std::string_view sceneName, float quality)
{
    const auto& property = std::any_cast<const ParallaxMappingProperty&>(Global::renderPassDatas->GetRenderPassProperty("G-BufferPass"));
    auto& prop = const_cast<ParallaxMappingProperty&>(property);
    prop.MipBias             = quality;
}
