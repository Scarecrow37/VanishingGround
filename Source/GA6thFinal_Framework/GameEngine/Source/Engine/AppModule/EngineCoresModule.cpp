#include "pch.h"

using namespace Global;

void EngineCoresModule::PreInitialize()
{
    SafeEngineCoresPtr::Engine::CreateEngineCores();
    ESceneManager::Engine::RegisterFileEvents();
    EGameObjectFactory::Engine::RegisterFileEvents();

    const SIZE& clientSize = UmApplication.GetClientSize();
    engineCore->Graphics.Initialize(engineCore->App.GetHwnd(), clientSize.cx, clientSize.cy, FeatureLevel::LEVEL_12_0);
}

void EngineCoresModule::ModuleUnInitialize()
{
    if constexpr (Application::IsEditor())
    {
        UmCommandManager.Clear();
    }
    ESceneManager::Engine::CleanupSceneManager();
    ETimeSystem::Engine::CleanUpInvokeFuntions();
    engineCore->ComponentFactory.UninitalizeComponentFactory();
    engineCore->Graphics.Device.Finalize();
    SafeEngineCoresPtr::Engine::DestroyEngineCores();
}