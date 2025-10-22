#include "pch.h"

using namespace Global;

void EngineCoresModule::PreInitialize()
{
    SafeEngineCoresPtr::Engine::CreateEngineCores();
    ESceneManager::Engine::RegisterFileEvents();
    EGameObjectFactory::Engine::RegisterFileEvents();
    
    const SIZE& clientSize = UmApplication.GetClientSize();
    UmPreferences.Initialize();
    bool isRayTracing = UmPreferences.IsRayTracing();
    UmGraphics.Initialize(engineCore->App.GetHwnd(), clientSize.cx, clientSize.cy, FeatureLevel::LEVEL_12_0, IS_EDITOR, isRayTracing);
}

void EngineCoresModule::ModuleUnInitialize()
{
    if constexpr (Application::IsEditor())
    {
        UmCommandManager.Clear();
        EditorHierarchyTool::CleanupEditorObjects();
    }
    ESceneManager::Engine::CleanupSceneManager();
    ETimeSystem::Engine::CleanUpInvokeFuntions();
    engineCore->ComponentFactory.UninitalizeComponentFactory();
    UmGraphics.Finalize();
    SafeEngineCoresPtr::Engine::DestroyEngineCores();
}