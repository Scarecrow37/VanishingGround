#include "pch.h"
#include "GraphicsCore.h"

void GraphicsCore::Initialize(HWND hwnd, UINT width, UINT height, FEATURE_LEVEL feature)
{
    Device.SetUpDevice(hwnd, width, height, feature);
    ViewManager.Initialize();
    Device.Initialize(); 
    Device.ResetCommands();
    Renderer.Initialize();

    auto commandList = Device.GetCommandList().Get();
    commandList->Close();
    Device.RegisterCommand(commandList,MESH_RENDER_LIST);
    Device.ExecuteCommand(MESH_RENDER_LIST);
    Device.GPUSync();

    UmDevice.ResetCommands();
    UmDevice.ResetComputeCommands();
}

void GraphicsCore::UpdateAnimation(const float deltaTime) {}

void GraphicsCore::Update()
{
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