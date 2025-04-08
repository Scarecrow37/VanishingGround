#include "pch.h"

using namespace Global;

void EngineCoresModule::PreInitialize()
{
    SafeEngineCoresPtr::Engine::CreateEngineCores();
    //engineCore->ComponentFactory.InitalizeComponentFactory();
    engineCore->SceneManager.CreateScene("EmptyScene");
    engineCore->SceneManager.LoadScene("EmptyScene");
}

void EngineCoresModule::PreUnInitialize()
{
    ESceneManager::Engine::CleanupSceneManager();
    engineCore->ComponentFactory.UninitalizeComponentFactory();
    SafeEngineCoresPtr::Engine::DestroyEngineCores();
}

