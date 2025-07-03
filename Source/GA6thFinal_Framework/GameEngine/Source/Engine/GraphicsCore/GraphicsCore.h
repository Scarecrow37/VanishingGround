#pragma once

#include "Enums.h"
#include "Structs.h"

#include "Camera.h"
#include "Device.h"
#include "Texture.h"
#include "ShaderBuilder.h"

#include "Renderer.h"
#include "AnimationCore.h"
#include "LightCore.h"

#include "MultiRenderTargetManager.h"
#include "ResourceManager.h"
#include "ViewManager.h"
#include "ParticleManager.h"
#include "DXResourceManager.h"

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
    DXResourceManager        DXResourceManager;
    DebugDrawCore            DebugDrawCore;

public:
    void Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature);
    void UpdateAnimation(const float deltaTime);
    void Update(const float deltaTime);
    void Render();
    void Flip();
};