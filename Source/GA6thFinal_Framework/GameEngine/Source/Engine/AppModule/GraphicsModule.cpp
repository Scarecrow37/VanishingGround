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
                                             {".png", ".dds", ".fbx", ".hdr", ".UmModel", ".sfont", ".jpg", ".inl"});

    UmFileSystem.RegisterFileEventSubscriber(&UmParticleSerializer, {".vfx"});

    RenderTechniqueFlag lightingFlag = RenderTechniqueFlag::NONE;
    lightingFlag = _israytracing ? RenderTechniqueFlag::RAY_TRACING_TECH : RenderTechniqueFlag::LIGHTING_TECH | RenderTechniqueFlag::SSR_TECH;

    RenderTechniqueFlag defaultFlag = RenderTechniqueFlag::SKY_BOX_TECH | lightingFlag | 
                               RenderTechniqueFlag::SSGI_TECH |
                               RenderTechniqueFlag::VOLUMETRIC_FOG_TECH |
                               RenderTechniqueFlag::PARTICLE_TECH |
                               RenderTechniqueFlag::BLOOM_TECH |
                               RenderTechniqueFlag::FXAA_TECH |
                               RenderTechniqueFlag::UI_TECH |
                               RenderTechniqueFlag::FONT_TECH;

    RenderTechniqueFlag gameSceneFlag = defaultFlag | RenderTechniqueFlag::SCENE_TRANSITION_TECH;
    UmGraphics.AddRenderScene("Game", gameSceneFlag);

    if constexpr (IS_EDITOR)
    {
        RenderTechniqueFlag editorSceneFlag = defaultFlag | RenderTechniqueFlag::EDITOR_DRAW_TECH;
        UmGraphics.AddRenderScene("Editor", editorSceneFlag);

        RenderTechniqueFlag modelViewerSceneFlag = RenderTechniqueFlag::SKY_BOX_TECH | lightingFlag;
        UmGraphics.AddRenderScene("ModelViewer", modelViewerSceneFlag);

        RenderTechniqueFlag particleEditorSceneFlag = RenderTechniqueFlag::PARTICLE_TECH | RenderTechniqueFlag::LIGHTING_TECH | RenderTechniqueFlag::BLOOM_TECH;
        UmGraphics.AddRenderScene("ParticleEditor", particleEditorSceneFlag);
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