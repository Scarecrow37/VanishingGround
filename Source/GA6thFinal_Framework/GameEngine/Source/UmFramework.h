#pragma once

//constexpr
#ifdef UMREALSCRIPTS_EXPORT
#define _SCRIPTS_PROJECT
constexpr bool IS_SCRIPTS_PROJECT = true; 
#else
constexpr bool IS_SCRIPTS_PROJECT = false;
#endif 

#ifdef _DEBUG
constexpr bool IS_DEBUG = true;
#else
constexpr bool IS_DEBUG = false;
#endif

#ifdef _UMEDITOR
constexpr bool IS_EDITOR = true;
#else
constexpr bool IS_EDITOR = false;
#endif

// 프로젝트 설정 파일들 모아두는 폴더
constexpr const wchar_t* PROJECT_SETTING_PATH = L"ProjectSettings"; 
// 에셋 파일들 모아두는 폴더
constexpr const wchar_t* ASSET_FOLDER_NAME = L"Assets";
// 문자열 null을 명시적으로 표시하기 위한 값
constexpr const char* STR_NULL = "null";

#define DIRECTX_TOOLKIT_IMPORT

//WINDOW SDK
#include <Windows.h>
#include <dbghelp.h>
#include <directx/d3dx12.h>
#include <dxgi1_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <directxtk12/SimpleMath.h>
#include <directxtk12/SpriteBatch.h>
#include <directxtk12/SpriteFont.h>
#include <directxtk12/ResourceUploadBatch.h>
#include <wrl.h>

#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "d3dcompiler")

using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace Microsoft::WRL;

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
#include <typeindex>
#include <string_view>

//ThirdParty
#include <pugixml.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <assimp/postprocess.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Engine/Imgui/imgui.h"
#include "Engine/Imgui/imgui_stdlib.h"
#include "Engine/Imgui/imgui_impl_win32.h"
#include "Engine/Imgui/imgui_impl_dx12.h"
#include "Engine/Imgui/imgui_node_editor.h"
#include "Engine/Imgui/imgui_internal.h"

#include "Engine/Imgui/ImGuizmo.h"
#include "Engine/Imgui/ImSequencer.h"
#include "Engine/Imgui/ImZoomSlider.h"
#include "Engine/Imgui/ImCurveEdit.h"
#include "Engine/Imgui/GraphEditor.h"

//Utility
#include "Engine/Utility/LogLevel.h"
#include "Engine/Utility/utfHelper.h"
#include "Engine/Utility/DumpUtility.h"
#include "Engine/Utility/dllUtility.h"
#include "Engine/Utility/ImGuiHelper.h"
#include "Engine/Utility/stlHelper.h"
#include "Engine/Utility/EditorHelper.h"
#include "Engine/Utility/Mathf.h"

//Class Core
#include "Engine/ClassCore/TProperty.hpp"
#include "Engine/ClassCore/ReflectHelper.h"

//Application Core
#include "Engine/AppModule/Interface/IAppModule.h"
#include "Engine/ApplicationCore/Application.h"

//FileSystem Module
#include "Engine/FileSystem/Type/FileDataType.h"
#include "Engine/Utility/FileHelper.h"
#include "Engine/FileSystem/Interface/IFileEventProcesser.h"
#include "Engine/FileSystem/Extra/FileObserver.h"
#include "Engine/FileSystem/Extra/FileContext.h"
#include "Engine/FileSystem/System/FileSystem.h"
#include "Engine/FileSystem/Extra/FileEventNotifier.h"
#include "Engine/FileSystem/FileSystemModule.h"


//Graphics
#include "Engine/GraphicsCore/GraphicsCore.h"

//Engine Core
#include "Engine/EngineCore/EngineLogger.h"
#include "Engine/EngineCore/TimeSystem.h"
#include "Engine/EngineCore/GameObjectFactory.h"
#include "Engine/EngineCore/ComponentFactory.h"
#include "Engine/EngineCore/SceneManager.h"
#include "Engine/CommandCore/CommandManager.h"
#include "Engine/EngineCore/EngineCores.h" 

//Editor Core
#include "Engine/EditorCore/EditorEnum.h"
#include "Engine/EditorCore/EditorGui.h"
#include "Engine/EditorCore/EditorTool.h"
#include "Engine/EditorCore/EditorMenuBar.h"
#include "Engine/EditorCore/EditorDockSpace.h"
#include "Engine/EditorCore/PopupBox/EditorPopupBoxSystem.h"
#include "Engine/EditorCore/EditorModule.h"

//Game Core
#include "Engine/GameCore/Transform/Transform.h"
#include "Engine/GameCore/GameObject/GameObject.h"
#include "Engine/GameCore/Component/Component.h"
#include "Engine/GameCore/Component/MissingComponent.h"

//Application Module
#include "Engine/AppModule/EngineCoresModule.h"
#include "Engine/AppModule/ImGuiDX12Module.h"

//DragDropTypes
#include "Editor/DragDropTypes/DragDropTransform.h"
#include "Editor/DragDropTypes/DragDropAsset.h"

//컴포넌트는 접근 안하는 헤더들
#ifndef _SCRIPTS_PROJECT
//Editor Tools
#include "Editor/Tool/Debug/EditorDebugTool.h"
#include "Editor/Tool/AssetBrowser/EditorAssetBrowserTool.h"
#include "Editor/Tool/Inspector/EditorInspectorTool.h"
#include "Editor/Tool/Hierarchy/EditorHierarchyTool.h"
#include "Editor/Tool/Scene/EditorSceneTool.h"
#include "Editor/Tool/Log/EditorLogsTool.h"
#include "Editor/Tool/Model/EditorModelTool.h"
#include "Editor/Tool/Command/EditorCommandTool.h"

#include "Editor/Tool/ScriptTest/ScriptTestEditor.h"    // 테스트용. 추후 제거 필요

// Editor Menu
#include "Editor/Menu/Project/EditorProjectMenu.h"
#include "Editor/Menu/Window/EditorWindowMenu.h"
#include "Editor/Menu/Setting/EditorSettingMenu.h"
#include "Editor/Menu/Scene/EditorSceneMenu.h"
#include "Editor/Menu/Project/EditorBuildSettingMenu.h"
#endif
