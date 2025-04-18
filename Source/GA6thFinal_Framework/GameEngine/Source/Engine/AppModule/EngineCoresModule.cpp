#include "pch.h"

using namespace Global;

void EngineCoresModule::PreInitialize()
{
    SafeEngineCoresPtr::Engine::CreateEngineCores();
    //engineCore->ComponentFactory.InitalizeComponentFactory();
    engineCore->Graphics.Initialize(engineCore->App.GetHwnd(), 1920, 1080, FEATURE_LEVEL::LEVEL_12_0);

    engineCore->SceneManager.CreateScene("Empty Scene");
    engineCore->SceneManager.LoadScene("Empty Scene");

    auto gameObject = NewGameObject<GameObject>("Test").lock();
    auto& test = gameObject->AddComponent<StaticMeshRenderer>();
}

void EngineCoresModule::ModuleUnInitialize()
{
    ESceneManager::Engine::CleanupSceneManager();
    engineCore->ComponentFactory.UninitalizeComponentFactory();
    //engineCore->Graphics.ResourceManager.Clear();
    engineCore->Graphics.Device.Finalize();
    SafeEngineCoresPtr::Engine::DestroyEngineCores();
}