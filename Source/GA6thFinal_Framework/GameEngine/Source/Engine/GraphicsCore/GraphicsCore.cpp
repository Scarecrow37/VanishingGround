#include "pch.h"
#include "GraphicsCore.h"

void GraphicsCore::Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature)
{
    Device.SetUpDevice(hwnd, width, height, feature);
    ViewManager.Initialize();
    Device.Initialize(); 
    Device.ResetCommands();
    ParticleManager.Initialize(MAX_PARTICLE);
    Renderer.Initialize();

    auto commandList = Device.GetCommandList();
    commandList->Close();

    auto imguiCommandList = Device.GetImguiCommandList();
    imguiCommandList->Close();

    Device.RegisterCommand(commandList,RENDER_LIST);
    Device.ExecuteCommand(RENDER_LIST);
    Device.GPUSync();

    Device.ResetCommands();
    Device.ResetComputeCommands();

    DebugDrawCore.Initialize();
 }

void GraphicsCore::UpdateAnimation(const float deltaTime)
{
    AnimationCore.Update(deltaTime);
}

void GraphicsCore::Update(const float deltaTime)
{
    ParticleManager.Update(deltaTime);
    LightCore.Update(deltaTime);
    Renderer.Update();
}

void GraphicsCore::Render()
{
    DebugDrawCore.Render();
    Renderer.Render();
}

void GraphicsCore::Flip()
{
    Renderer.Flip();
}