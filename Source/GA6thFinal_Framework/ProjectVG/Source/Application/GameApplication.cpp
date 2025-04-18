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
    

    //에디터 매니저 등록
    _editorManager = AddModule<EditorModule>();

    //추가할 에디터 작성
    /* Tool */
    _editorManager->RegisterEditorObject<EditorDebugTool>();
    _editorManager->RegisterEditorObject<EditorHierarchyTool>();
    _editorManager->RegisterEditorObject<EditorInspectorTool>();
    _editorManager->RegisterEditorObject<EditorSceneTool>();
    _editorManager->RegisterEditorObject<EditorAssetBrowserTool>();
    _editorManager->RegisterEditorObject<EditorLogsTool>();

    //김시우 테스트용
    _editorManager->RegisterEditorObject<ScriptTestEditor>();

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
    _editorManager->RegisterEditorObject<EditorMenuFileSystemSetting>();
    // Scene
    _editorManager->RegisterEditorObject<EditorSceneMenuGameObject>();
    _editorManager->RegisterEditorObject<EditorSceneMenuScenes>();

    // TestMenu. 추후 삭제 필요
    _editorManager->RegisterEditorObject<SampleMenu>();
    _editorManager->RegisterEditorObject<SampleMenu2>();

    // 만든 EditorModule에 MySampleEditorTool 등록
    _editorManager->RegisterEditorObject<MySampleEditorTool>();
}

GameApplication::~GameApplication()
{

}

MySampleEditorTool::MySampleEditorTool()
{
    /* 필수 구현 항목*/
    SetLabel("MySampleTool"); // 툴 이름

    /* 선택적 옵션 */
    SetSize(ImVec2(500, 300));            // 초기 사이즈
    SetPos(ImVec2(300, 100));             // 초기 위치
    SetCallOrder(0);                      // 호출 순서 (0일시 생략 가능)
    SetWindowFlag(ImGuiWindowFlags_None); // 윈도우 플래그 (ImGuiWindowFlags_NoCollapse는 항상 활성화)
}

// MySampleEditorTool.cpp
void MySampleEditorTool::OnFrame()
{
    ImGuiHelper::TooltipMarker("MySampleTool");
    ImGui::Text("Hello, World!");
    ImGuiHelper::LoadingSpinner(10.0f, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
}

void MySampleEditorTool::OnPopup()
{
    if (ImGui::MenuItem("Sample1"))
    {
        UmEngineLogger.Log(LogLevel::LEVEL_DEBUG, "Clicked Sample1");
    }
    if (ImGui::MenuItem("Sample2"))
    {
        UmEngineLogger.Log(LogLevel::LEVEL_DEBUG, "Clicked Sample2");
    }
}