#pragma once

#include "Enums.h"
#include "Structs.h"

#include "Camera.h"
#include "Device.h"
#include "Renderer.h"
#include "Texture.h"

#include "ResourceManager.h"
#include "ViewManager.h"

#include "Defines.h"
#include "Externs.h"

class Model;
class GraphicsCore
{
public:
    Device          Device;
    Renderer        Renderer;
    ViewManager     ViewManager;
    ResourceManager ResourceManager;
    //Camera          MainCamera;

public:
    void Initialize(HWND hwnd, UINT width, UINT height, FEATURE_LEVEL feature);
    void UpdateAnimation(const float deltaTime);
    void Update();
    void Render();
    void Flip();
};