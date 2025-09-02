#include "pch.h"
#include "GraphicsModule.h"
#include "Engine/GraphicsCore/RendererFileEvent.h"
#include "Engine/GraphicsCore/ParticleEffectSerializer.h"

bool _israytracing = false;

GraphicsModule::GraphicsModule()
{
}

GraphicsModule::~GraphicsModule()
{
}

void GraphicsModule::PreInitialize()
{
    _rendererFileEvent = std::make_unique<RendererFileEvent>();
    UmFileSystem.RegisterFileEventSubscriber(_rendererFileEvent.get(),
                                             {".png", ".dds", ".fbx", ".hdr", ".UmModel", ".sfont", ".jpg"});

    UmFileSystem.RegisterFileEventSubscriber(&UmParticleSerializer, {".vfx"});

    RenderTechniqueFlag lightingFlag = RenderTechniqueFlag::NONE;
    lightingFlag = _israytracing ? RenderTechniqueFlag::RAY_TRACING_TECH | RenderTechniqueFlag::VOLUMETRIC_FOG_TECH
                                                     : RenderTechniqueFlag::PBR_TECH | RenderTechniqueFlag::SSR_TECH | RenderTechniqueFlag::VOLUMETRIC_FOG_TECH;

    RenderTechniqueFlag flag = RenderTechniqueFlag::SKY_BOX_TECH | lightingFlag | 
                               RenderTechniqueFlag::PARTICLE_TECH |
                               RenderTechniqueFlag::BLOOM_TECH | RenderTechniqueFlag::UI_TECH |
                               RenderTechniqueFlag::FONT_TECH;

    UmGraphics.AddRenderScene("Game", flag);

    if constexpr (IS_EDITOR)
    {
        flag |= RenderTechniqueFlag::EDITOR_DRAW_TECH;
        UmGraphics.AddRenderScene("Editor", flag);

        flag = RenderTechniqueFlag::SKY_BOX_TECH | lightingFlag;
        UmGraphics.AddRenderScene("ModelViewer", flag);

        flag = RenderTechniqueFlag::PARTICLE_TECH | RenderTechniqueFlag::EDITOR_DRAW_TECH | RenderTechniqueFlag::BLOOM_TECH;
        UmGraphics.AddRenderScene("ParticleEditor", flag);
    }
    else
    {
        UmGraphics.SetCurrentScene("Game");
    }    
}

void GraphicsModule::ModuleInitialize()
{
}

void GraphicsModule::PreUnInitialize()
{
}

void GraphicsModule::ModuleUnInitialize()
{
}