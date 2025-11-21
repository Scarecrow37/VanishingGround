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
// 빌드 설정 파일 모아두는 폴더
constexpr const wchar_t* BUILD_SETTING_PATH = L"BuildSettings"; 
// 에셋 파일들 모아두는 폴더
constexpr const wchar_t* ASSET_FOLDER_NAME = L"Assets";
// 패킹 파일 이름
constexpr const wchar_t* PACKER_FILE_NAME = L"Assets.UmPack";
// 문자열 null을 명시적으로 표시하기 위한 값
constexpr const char* STR_NULL = "null";

//WINDOW SDK
#define NOMINMAX
#include <Windows.h>
#include <wrl.h>
#include <Xinput.h>
#include <xaudio2.h>

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
#include <map>
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
#include <concurrent_queue.h>
#include <random>
#include <algorithm>
#include <numeric>
#include <variant>
#include <regex>

//ThirdParty
#include "pugixml/pugixml.hpp"
#include "UmYaml-cpp.h"

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../ImGuiThirdParty/imgui.h"
#include "../ImGuiThirdParty/imgui_impl_dx12.h"
#include "../ImGuiThirdParty/imgui_impl_win32.h"
#include "../ImGuiThirdParty/imgui_internal.h"
#include "../ImGuiThirdParty/imgui_node_editor.h"
#include "../ImGuiThirdParty/imgui_stdlib.h"

#include "../ImGuiThirdParty/GraphEditor.h"
#include "../ImGuiThirdParty/ImCurveEdit.h"
#include "../ImGuiThirdParty/ImGuizmo.h"
#include "../ImGuiThirdParty/ImSequencer.h"
#include "../ImGuiThirdParty/ImZoomSlider.h"

//namespace
using namespace Microsoft::WRL;

// Graphics
#include "../GraphicsEngine/Graphics.h"

//Class Core
#include "Engine/ClassCore/Delegate.hpp"
#include "Engine/ClassCore/TProperty.hpp"
#include "Engine/ClassCore/ReflectHelper.h"

//Utility
#include "Engine/Utility/Random.h"
#include "Engine/Utility/LogLevel.h"
#include "Engine/Utility/utfHelper.h"
#include "Engine/Utility/DumpUtility.h"
#include "Engine/Utility/dllUtility.h"
#include "Engine/Utility/stlHelper.h"
#include "Engine/Utility/EditorHelper.h"
#include "Engine/Utility/Mathf.h"
#include "Engine/Utility/OpenXLSXHelper.h"
#include "Engine/Utility/BoxSpacing.h"
#include "Engine/Utility/ImGuiHelper.h"

//Input Module
#include "../InputModule/InputModule.h"

//Audio Module
#include "../AudioModule/AudioModule.h"

//Packer Module
#include "../PackerModule/PackerModule.h"

//Application Core
#include "Engine/AppModule/Interface/IAppModule.h"
#include "Engine/ApplicationCore/Application.h"

//FileSystem Module
#include "Engine/FileSystem/Type/FileSystemSetting.h"
#include "Engine/FileSystem/Type/FileGuidPath.h"
#include "Engine/FileSystem/Type/FileData.h"
#include "Engine/FileSystem/Type/FileInformation.h"

#include "Engine/Utility/FileHelper.h"
#include "Engine/FileSystem/Interface/IFileEventProcesser.h"
#include "Engine/FileSystem/Event/FileEventObserver.h"
#include "Engine/FileSystem/Context/FileContext.h"
#include "Engine/FileSystem/System/FileSystem.h"
#include "Engine/FileSystem/Event/FileEventSubscriber.h"
#include "Engine/FileSystem/FileSystemModule.h"

//Audio Core
#include "Engine/AudioCore/Manager/AudioManager.h"

//User Interface Module
#include "../UserInterfaceModule/UserInterfaceModule.h"
#include "Engine/EngineCore/UIManager.h"

//CommandCore
#include "Engine/CommandCore/CommandManager.h"

//Watcher
#include "Engine/WatcherCore/System/WatcherSystem.h"

//Engine Core
#include "Engine/GraphicsCore/ParticleEffectSerializer.h"
#include "Engine/EngineCore/EngineLogger.h"
#include "Engine/EngineCore/TimeSystem.h"
#include "Engine/EngineCore/GameObjectFactory.h"
#include "Engine/EngineCore/ComponentFactory.h"
#include "Engine/EngineCore/SceneManager.h"
#include "Engine/EngineCore/FactoryRegister.h"
#include "Engine/EngineCore/GizmoManager.h"
#include "Engine/EngineCore/PreferencesSystem.h"
#include "Engine/EngineCore/ImGuiDebuggerWindow.h"
#include "Engine/EngineCore/EngineCores.h" 

//Editor Core
#include "Engine/EditorCore/Interface/IEditorObject.h"
#include "Engine/EditorCore/Interface/IEditorCycle.h"
#include "Engine/EditorCore/Utillity/EditorDragState.h"
#include "Engine/EditorCore/Gui/EditorGui.h"
#include "Engine/EditorCore/Gui/Tool/EditorTool.h"
#include "Engine/EditorCore/Gui/Menu/EditorMenu.h"
#include "Engine/EditorCore/Gui/PopupBox/EditorPopupBox.h"
#include "Engine/EditorCore/Gui/DockWindow/EditorDockWindow.h"
#include "Engine/EditorCore/Gui/Sequencer/EditorTimelineTrackSequencer.h"
#include "Engine/EditorCore/System/EditorPopupBoxSystem.h"
#include "Engine/EditorCore/System/EditorGuiSystem.h"
#include "Engine/EditorCore/EditorModule.h"

//Input Auto
#include "Engine/ClassCore/InputAuto.h"

//Game Core
#include "Engine/GameCore/Transform/Transform.h"
#include "Engine/GameCore/GameObject/GameObject.h"
#include "Engine/GameCore/Component/Component.h"
#include "Engine/GameCore/Component/MissingComponent.h"
#include "Engine/GameCore/Component/UIBaseComponent.h"
#include "Engine/GameCore/Component/UIComponent.h"
#include "Engine/GameCore/Component/UIRoot.h"
#include "Engine/GameCore/Component/UINavigationComponent.h"
#include "Engine/GameCore/Component/UISFXNavigationComponent.h"
#include "Engine/GameCore/InputReciver/InputReceiver.h"

//Application Module
#include "Engine/AppModule/EngineCoresModule.h"
#include "Engine/AppModule/ImGuiDX12Module.h"
#include "Engine/AppModule/GraphicsModule.h"
#include "Engine/AppModule/AudioModule.h"

//DragDropTypes
#include "Editor/DragDropTypes/DragDropTransform.h"
#include "Editor/DragDropTypes/DragDropAsset.h"

//Timeline System
#include "Engine/TimelineCore/Context/TimelineEventContext.h"
#include "Engine/TimelineCore/Context/Audio/TimelineAudioEventContext.h"
#include "Engine/TimelineCore/Context/Particle/TimelineParticleEventContext.h"
#include "Engine/TimelineCore/Context/Input/TimelineInputEventContext.h"
#include "Engine/TimelineCore/Track/TimelineEventTrack.h"
#include "Engine/TimelineCore/Animation/AnimationEventTrack.h"

//컴포넌트는 접근 안하는 헤더들
#ifndef _SCRIPTS_PROJECT

//Editor Tools
#include "Editor/Tool/Debug/EditorDebugTool.h"
#include "Editor/Tool/AssetBrowser/EditorAssetBrowserTool.h"
#include "Editor/Tool/Inspector/EditorInspectorTool.h"
#include "Editor/Tool/Hierarchy/EditorHierarchyTool.h"
#include "Editor/Tool/Hierarchy/HierarchyFindTool.h"
#include "Editor/Tool/Scene/EditorSceneTool.h"
#include "Editor/Tool/GameView/EditorGameView.h"
#include "Editor/Tool/Log/EditorLogsTool.h"
#include "Editor/Tool/Model/EditorModelDetails.h"
#include "Editor/Tool/Model/EditorModelTool.h"
#include "Editor/Tool/Model/EditorModelHierarchy.h"
#include "Editor/Tool/Model/EditorAnimationTrackTool.h"
#include "Editor/Tool/Command/EditorCommandTool.h"
#include "Editor/Tool/RenderPassData/EditorRenderPassData.h"

#include "Editor/Tool/ParticleEffect/EditorParticleEffectDetails.h"
#include "Editor/Tool/ParticleEffect/EditorParticleEffectViewer.h"
#include "Editor/Tool/ParticleEffect/EditorParticleEffectHierarchy.h"

// Editor Menu
#include "Editor/Menu/Project/EditorProjectMenu.h"
#include "Editor/Menu/Window/EditorWindowMenu.h"
#include "Editor/Menu/Setting/EditorSettingMenu.h"
#include "Editor/Menu/Scene/EditorSceneMenu.h"
#include "Editor/Menu/Project/EditorBuildSettingMenu.h"
#include "Editor/Menu/Play/EditorPlayMenu.h"
#include "Editor/Menu/Model/EditorModelMenu.h"
#include "Editor/Menu/Game/EditorGameMenu.h"

#endif

#include "Editor/SceneGizmo/SceneGizmo.h"