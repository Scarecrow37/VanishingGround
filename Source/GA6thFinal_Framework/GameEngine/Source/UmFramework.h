#pragma once
#define _CRT_SECURE_NO_WARNINGS 1

//constexpr
#ifdef UMREALSCRIPTS_EXPORT
#define SCRIPTS_PROJECT true;
constexpr bool IS_SCRIPTS_PROJECT = true;
#else
constexpr bool IS_SCRIPTS_PROJECT = false;
#endif 

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
#include <source_location>
#include <any>
#include <typeinfo>
#include <ranges>
#include <concepts>
#include <queue>
#include <stack>

//ThirdParty
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
#include "Engine/EditorCore/EditorManager.h"

//Game Core
#include "Engine/GameCore/Transform/Transform.h"
#include "Engine/GameCore/GameObject/GameObject.h"
#include "Engine/GameCore/Component/Component.h"

//Application Module
#include "Engine/AppModule/EngineCoresModule.h"
#include "Engine/Graphics/GraphicsModule.h"
#include "Engine/AppModule/ImGuiDX11Module.h"
