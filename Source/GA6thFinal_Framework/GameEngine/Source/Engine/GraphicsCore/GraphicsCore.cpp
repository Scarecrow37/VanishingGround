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
    Device.RegisterCommand(commandList);
    Device.ExecuteCommand();
    Device.GPUSync();

    MainCamera.SetupPerspective(45.f, static_cast<float>(width) / height, 0.1f, 100.f);
    MainCamera.SetPosition({0.f, 0.f, -5.f});
}

void GraphicsCore::UpdateAnimation(const float deltaTime)
{
}

void GraphicsCore::Update()
{
    // 임시 카메라
    MainCamera.Update();
    Renderer.Update();
}

void GraphicsCore::Render()
{
    Renderer.Render();
}