#pragma once

namespace Global
{
    extern Device*                      device;
    extern Renderer*                    renderer;
    extern CommandController*           commandController;
    extern DXResourceManager*           dxResourceManager;
    extern MultiRenderTargetManager*    multiRenderTargetManager;
    extern ResourceManager*             resourceManager;
    extern ViewManager*                 viewManager;
    extern AnimationCore*               animationCore;
    extern LightCore*                   lightCore;
    extern ParticleManager*             particleManager;
    extern DebugDrawCore*               debugDrawCore;
    extern RenderPassDatas*             renderPassDatas;
    extern ModuleManager*               moduleManager;
    extern PipelineStateManager*        pipelineStateManager;
    extern ThreadPool*                  threadPool;
    extern bool                         isRayTracing;
    extern SceneTransitionCore*         sceneTransitionCore;
    extern D3D12_GPU_DESCRIPTOR_HANDLE* dummyTextureHandle;
}