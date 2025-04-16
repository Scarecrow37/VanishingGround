#include "pch.h"

using namespace Global;

void EngineCoresModule::PreInitialize()
{
    SafeEngineCoresPtr::Engine::CreateEngineCores();
    //engineCore->ComponentFactory.InitalizeComponentFactory();
    engineCore->SceneManager.CreateScene("Empty Scene");
    engineCore->SceneManager.LoadScene("Empty Scene");
}

void EngineCoresModule::ModuleUnInitialize()
{
    ESceneManager::Engine::CleanupSceneManager();
    engineCore->ComponentFactory.UninitalizeComponentFactory();
    SafeEngineCoresPtr::Engine::DestroyEngineCores();
}

