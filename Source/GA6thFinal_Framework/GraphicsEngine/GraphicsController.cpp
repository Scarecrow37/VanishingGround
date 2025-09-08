#include "pch.h"
#include "GraphicsController.h"

// render framework
#include "RenderScene.h"

// render technique
#include "SSRTechnique.h"
#include "PBRLitTechnique.h"

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
    auto         pbrTech     = renderScene->GetRenderTechnique<PBRLitTechnique>();
}
