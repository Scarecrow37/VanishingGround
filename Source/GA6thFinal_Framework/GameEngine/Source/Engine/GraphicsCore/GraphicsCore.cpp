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

    auto postProcessCommandList = Device.GetPostProcessCommandList();
    postProcessCommandList->Close();

    Device.RegisterCommand(commandList,MESH_RENDER_LIST);
    Device.ExecuteCommand(MESH_RENDER_LIST);
    Device.GPUSync();

    Device.ResetCommands();
    Device.ResetComputeCommands();
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
    Renderer.Render();
}

void GraphicsCore::Flip()
{
    Renderer.Flip();
}