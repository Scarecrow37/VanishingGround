#include "pch.h"
#include "GraphicsConstroller.h"

// render framework
#include "RenderScene.h"

// render technique
#include "SSRTechnique.h"
#include "PBRLitTechnique.h"

GraphicsConstroller::~GraphicsConstroller() {}

void GraphicsConstroller::SetSSR(std::string_view sceneName, bool enable) 
{
    RenderScene* renderScene = Global::renderer->GetRenderScene(sceneName);
    auto ssrTech = renderScene->GetRenderTechnique<SSRTechnique>();
    
    ssrTech->SetEnable(enable);
}

void GraphicsConstroller::SetSSAO(std::string_view sceneName, bool enable)
{
    RenderScene* renderScene = Global::renderer->GetRenderScene(sceneName);
    auto         pbrTech     = renderScene->GetRenderTechnique<PBRLitTechnique>();
}
