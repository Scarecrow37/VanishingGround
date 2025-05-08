#include "GameApplication.h"
#include "UmFramework.h"
#include "UmScripts.h"

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

    if constexpr (true == Application::IsEditor())
    {
        // 에디터 매니저 등록
        _editorModule = AddModule<EditorModule>();

        // 추가할 에디터 작성
        /* Tool */
        _editorModule->RegisterEditorObject<EditorDebugTool>();
        _editorModule->RegisterEditorObject<EditorHierarchyTool>();
        _editorModule->RegisterEditorObject<EditorInspectorTool>();
        _editorModule->RegisterEditorObject<EditorSceneTool>();
        _editorModule->RegisterEditorObject<EditorAssetBrowserTool>();
        _editorModule->RegisterEditorObject<EditorLogsTool>();
        _editorModule->RegisterEditorObject<EditorCommandTool>();
        _editorModule->RegisterEditorObject<EditorModelTool>();

        // 블루프린트 버그있음
        //_editorModule->RegisterEditorObject<EditorShaderGraph>();

        /* Menu */
        // Project
        _editorModule->RegisterEditorObject<EditorMenuProjectRoot>();
        _editorModule->RegisterEditorObject<EditorMenuScriptBuilder>();
        _editorModule->RegisterEditorObject<EditorBuildSettingMenu>();
        // Window
        _editorModule->RegisterEditorObject<EditorMenuTools>();
        // Setting
        _editorModule->RegisterEditorObject<EditorMenuDebug>();
        _editorModule->RegisterEditorObject<EditorMenuStyleEditor>();
        _editorModule->RegisterEditorObject<EditorMenuFileSystemSetting>();
        // Scene
        _editorModule->RegisterEditorObject<EditorSceneMenuGameObject>();
        _editorModule->RegisterEditorObject<EditorSceneMenuScenes>();
    }
}

GameApplication::~GameApplication()
{

}

void GameApplication::OnStartupComplete() 
{
}

void GameApplication::OnShutdownComplete() 
{

}
