#include "pch.h"

using namespace Global;

void EngineCoresModule::PreInitialize()
{
    SafeEngineCoresPtr::Engine::CreateEngineCores();
    ESceneManager::Engine::RegisterFileEvents();
    EGameObjectFactory::Engine::RegisterFileEvents();
    
    const SIZE& clientSize = UmApplication.GetClientSize();
    bool isRayTracing = UmPreferences.IsRayTracing();
    UmGraphics.Initialize(engineCore->App.GetHwnd(), clientSize.cx, clientSize.cy, FeatureLevel::LEVEL_12_0, IS_EDITOR, isRayTracing);
    UmPreferences.Initialize();
}

void EngineCoresModule::ModuleUnInitialize()
{
    if constexpr (Application::IsEditor())
    {
        UmCommandManager.Clear();
        EditorHierarchyTool::CleanupEditorObjects();
    }
    ETimeSystem::Engine::CleanUpInvokeFuntions();
    ESceneManager::Engine::CleanupSceneManager();
    EGameObjectFactory::Engine::Finalize();
    engineCore->ComponentFactory.UninitalizeComponentFactory();
    UmPreferences.Uninitialize();
    UmGraphics.Finalize();
    SafeEngineCoresPtr::Engine::DestroyEngineCores();
}