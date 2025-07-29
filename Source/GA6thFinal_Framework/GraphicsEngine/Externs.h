#pragma once

namespace Global
{
    extern Device*                   device;
    extern Renderer*                 renderer;
    extern CommandController*        commandController;
    extern DXResourceManager*        dxResourceManager;
    extern MultiRenderTargetManager* multiRenderTargetManager;
    extern ResourceManager*          resourceManager;
    extern ViewManager*              viewManager;
    extern AnimationCore*            animationCore;
    extern LightCore*                lightCore;
    extern ParticleManager*          particleManager;
    extern DebugDrawCore*            debugDrawCore;
    extern DebugDatas*               debugDatas;
}