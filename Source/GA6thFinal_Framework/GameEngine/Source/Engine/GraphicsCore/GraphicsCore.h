#pragma once

#include "Enums.h"
#include "Structs.h"
#include "Headers.h"
#include "Defines.h"
#include "Externs.h"

#include "DebugDrawCore.h"

class GraphicsCore
{
public:
    Device                   Device;
    Renderer                 Renderer;
    AnimationCore            AnimationCore;
    LightCore                LightCore;
    ViewManager              ViewManager;
    ResourceManager          ResourceManager;
    MultiRenderTargetManager MultiRenderTargetManager;
    ParticleManager          ParticleManager;
    AccelerationStructureManager AccelerationStructureManager;
    DXResourceManager        DXResourceManager;
    DebugDrawCore            DebugDrawCore;

public:
    void Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature);
    void UpdateAnimation(const float deltaTime);
    void Update(const float deltaTime);
    void Render();
    void Flip();
};