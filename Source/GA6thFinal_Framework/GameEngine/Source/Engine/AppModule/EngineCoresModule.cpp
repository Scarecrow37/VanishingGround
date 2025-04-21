#include "pch.h"

using namespace Global;

void EngineCoresModule::PreInitialize()
{
    SafeEngineCoresPtr::Engine::CreateEngineCores();
    ESceneManager::Engine::RegisterFileEvents();
    EGameObjectFactory::Engine::RegisterFileEvents();

    const SIZE& clientSize = UmApplication.GetClientSize();
    engineCore->Graphics.Initialize(engineCore->App.GetHwnd(), clientSize.cx, clientSize.cy, FEATURE_LEVEL::LEVEL_12_0);

    engineCore->SceneManager.CreateScene("Empty Scene");
    engineCore->SceneManager.LoadScene("Empty Scene");
}

void EngineCoresModule::ModuleUnInitialize()
{
    ESceneManager::Engine::CleanupSceneManager();
    engineCore->ComponentFactory.UninitalizeComponentFactory();
    //engineCore->Graphics.ResourceManager.Clear();
    engineCore->Graphics.Device.Finalize();
    SafeEngineCoresPtr::Engine::DestroyEngineCores();
}