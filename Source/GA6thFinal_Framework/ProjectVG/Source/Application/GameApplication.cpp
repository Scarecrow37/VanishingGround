#include "GameApplication.h"
#include "UmFramework.h"

#include "Source/EditorTools/EditorDebugView.h"
#include "Source/EditorTools/EditorHierarchyView.h"
#include "Source/EditorTools/EditorInspectorView.h"
#include "Source/EditorTools/EditorSceneView.h"
#include "Source/EditorTools/EditorAssetBrowser.h"
#include "Source/EditorTools/NodeEditor/EditorShaderGraph.h"
#include "Source/EditorTools/EditorLogsTool/EditorLogsTool.h"

#include "Source/EditorTools/EditorMenu/EditorProjectMenu.h"
#include "Source/EditorTools/EditorMenu/EditorWindowMenu.h"
#include "Source/EditorTools/EditorMenu/EditorSettingMenu.h"

#include "Source/TestEditor/ObjectTestEditor.h"
#include "Source/TestEditor/ReflectTestEditor.h"

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    GameApplication app;
    Application::MainEntry::Initialize(hInstance);
    Application::MainEntry::Run();
    Application::MainEntry::UnInitialize();
    return 0;
}

GameApplication::GameApplication()
{
    //클라이언트 기본 초기화 함수.
    SetStyleToWindowed();
    _clientSize = { 1920, 1080 };
    _windowName = L"Umreal Engine";
    
    //에디터 매니저 등록
    _editorManager = AddModule<EditorManager>();

    //추가할 에디터 작성
    /* Tool */
    _editorManager->RegisterEditorObject<EditorDebugView>();
    _editorManager->RegisterEditorObject<EditorHierarchyView>();
    _editorManager->RegisterEditorObject<EditorInspectorView>();
    _editorManager->RegisterEditorObject<EditorSceneView>();
    _editorManager->RegisterEditorObject<EditorAssetBrowser>();
    _editorManager->RegisterEditorObject<EditorLogsTool>();

    //김시우 테스트용
    _editorManager->RegisterEditorObject<ObjectTestEditor>();
    _editorManager->RegisterEditorObject<ReflectTestEditor>();

    //블루프린트 버그있음
    //_editorManager->RegisterEditorObject<EditorShaderGraph>();

    /* Menu */
    //Project
    _editorManager->RegisterEditorObject<EditorMenuScriptBuilder>();
    // Window
    _editorManager->RegisterEditorObject<EditorMenuTools>();
    // Setting
    _editorManager->RegisterEditorObject<EditorMenuDebug>();
    _editorManager->RegisterEditorObject<EditorMenuStyleEditor>();
}

GameApplication::~GameApplication()
{

}

