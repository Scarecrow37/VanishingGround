#pragma once

//constexpr
#ifdef UMREALSCRIPTS_EXPORT
#define SCRIPTS_PROJECT true;
constexpr bool IS_SCRIPTS_PROJECT = true; 
#else
constexpr bool IS_SCRIPTS_PROJECT = false;
#endif 

constexpr const wchar_t* PROJECT_SETTING_PATH = L"ProjectSetting"; // 프로젝트 설정 파일들 모아두는 폴더

//WINDOW SDK
#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>
#include <dbghelp.h>
#include <dxgi1_4.h>
#include <directxtk/simplemath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "Dbghelp.lib")

//CRT
#include <tchar.h>
    
//STL
#include <filesystem>
#include <iostream>
#include <exception>
#include <cstdio>
#include <memory>
#include <string>
#include <limits>
#include <array>
#include <vector>
#include <type_traits>
#include <functional> 
#include <unordered_map>
#include <unordered_set>
#include <source_location>
#include <any>
#include <typeinfo>
#include <ranges>
#include <concepts>
#include <queue>
#include <stack>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <system_error>

//ThirdParty
#include <pugixml.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "Engine/Imgui/imgui.h"
#include "Engine/Imgui/imgui_stdlib.h"
#include "Engine/Imgui/imgui_impl_win32.h"
#include "Engine/Imgui/imgui_impl_dx11.h"
#include "Engine/Imgui/imgui_impl_dx12.h"
#include "Engine/Imgui/imgui_node_editor.h"
#include "Engine/Imgui/imgui_internal.h"

//Utility
#include "Engine/Utility/LogLevel.h"
#include "Engine/Utility/utfHelper.h"
#include "Engine/Utility/DumpUtility.h"
#include "Engine/Utility/dllUtility.h"
#include "Engine/Utility/ImGuiHelper.h"
#include "Engine/Utility/stlHelper.h"
#include "Engine/Utility/Mathf.h"

//Class Core
#include "Engine/ClassCore/TProperty.hpp"
#include "Engine/ClassCore/ReflectHelper.h"

//Application Core
#include "Engine/AppModule/Interface/IAppModule.h"
#include "Engine/ApplicationCore/Application.h"

// FileSystem Module
#include "Engine/FileSystem/Type/FileDataType.h"
#include "Engine/Utility/FileHelper.h"
#include "Engine/FileSystem/Interface/IFileEventProcesser.h"
#include "Engine/FileSystem/Extra/FileObserver.h"
#include "Engine/FileSystem/Extra/FileContext.h"
#include "Engine/FileSystem/System/FileSystem.h"
#include "Engine/FileSystem/Extra/FileEventNotifier.h"
#include "Engine/FileSystem/FileSystemModule.h"

//Engine Core
#include "Engine/EngineCore/EngineLogger.h"
#include "Engine/EngineCore/TimeSystem.h"
#include "Engine/EngineCore/GameObjectFactory.h"
#include "Engine/EngineCore/ComponentFactory.h"
#include "Engine/EngineCore/SceneManager.h"
#include "Engine/EngineCore/EngineCores.h" 

//Editor Core
#include "Engine/EditorCore/EditorEnum.h"
#include "Engine/EditorCore/EditorBase.h"
#include "Engine/EditorCore/EditorTool.h"
#include "Engine/EditorCore/EditorMenuBar.h"
#include "Engine/EditorCore/EditorDockSpace.h"
#include "Engine/EditorCore/EditorModule.h"
//#include "Editor/Tool/AssetBrowser/"

//Game Core
#include "Engine/GameCore/Transform/Transform.h"
#include "Engine/GameCore/GameObject/GameObject.h"
#include "Engine/GameCore/Component/Component.h"

//Application Module
#include "Engine/AppModule/EngineCoresModule.h"
#include "Engine/Graphics/GraphicsModule.h"
#include "Engine/AppModule/ImGuiDX11Module.h"

//DragDropTypes
#include "Editor/DragDropTypes/DragDropTransform.h"

//컴포넌트는 접근 안하는 헤더들
#ifndef SCRIPTS_PROJECT
//Editor Tools
#include "Editor/Tool/Debug/EditorDebugTool.h"
#include "Editor/Tool/AssetBrowser/EditorAssetBrowserTool.h"
#include "Editor/Tool/Hierarchy/EditorHierarchyTool.h"
#include "Editor/Tool/Inspector/EditorInspectorTool.h"
#include "Editor/Tool/Scene/EditorSceneTool.h"
#include "Editor/Tool/Log/EditorLogsTool.h"

#include "Editor/Tool/ScriptTest/ScriptTestEditor.h"    // 테스트용. 추후 제거 필요

// Editor Menu
#include "Editor/Menu/Project/EditorProjectMenu.h"
#include "Editor/Menu/Window/EditorWindowMenu.h"
#include "Editor/Menu/Setting/EditorSettingMenu.h"
#include "Editor/Menu/Scene/EditorSceneMenu.h"
#endif