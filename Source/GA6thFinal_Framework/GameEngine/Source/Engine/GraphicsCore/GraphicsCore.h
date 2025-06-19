#pragma once

#include "Enums.h"
#include "Structs.h"

#include "AnimationCore.h"
#include "Camera.h"
#include "Device.h"
#include "LightCore.h"
#include "Renderer.h"
#include "Texture.h"
#include "ShaderBuilder.h"

#include "MultiRenderTargetManager.h"
#include "ResourceManager.h"
#include "ViewManager.h"

#include "Defines.h"
#include "Externs.h"

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

public:
    void Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature);
    void UpdateAnimation(const float deltaTime);
    void Update(const float deltaTime);
    void Render();
    void Flip();
};