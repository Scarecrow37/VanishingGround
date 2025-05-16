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

#ifdef _UMEDITOR
    // 에디터 매니저 등록
    _editorModule = AddModule<EditorModule>();
    BuildRootDock();
    BuildSceneDock();
    BuildModelDock();
#endif // _UMEDITOR
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

void GameApplication::BuildRootDock() 
{
    auto& dockSystem = _editorModule->GetDockWindowSystem();

    _rootDock = dockSystem.RegisterDockWindow("RootDock");

    ImGuiWindowClass imguiwindowClass;
    imguiwindowClass.ClassId                = ImHashStr("RootDockID"); // 윈도우 ID값 (그냥 대충 ImHashStr을 사용하여 생성)
    imguiwindowClass.DockingAllowUnclassed  = false; // 허용되지 않은 윈도우의 도킹을 허용할 것인가
    imguiwindowClass.DockingAlwaysTabBar    = false; // 도킹 탭바를 항상 표시할 것인가

    int imguiWindowFlag = 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;

    int imguiDockNodeFlag = 
        ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;

    int dockWindowFlag = 
        EditorDockWindow::DOCKWINDOW_FLAGS_FULLSCREEN | EditorDockWindow::DOCKWINDOW_FLAGS_PADDING;

    _rootDock->SetWindowClass(imguiwindowClass);
    _rootDock->SetImGuiWindowFlag(imguiWindowFlag);
    _rootDock->SetImGuiDockNodeFlag(imguiDockNodeFlag);
    _rootDock->SetDockWindowFlags(dockWindowFlag);

    _rootDock->RegisterGui<EditorMenuProjectRoot>();
    _rootDock->RegisterGui<EditorMenuScriptBuilder>();
    _rootDock->RegisterGui<EditorBuildSettingMenu>(); 
    _rootDock->RegisterGui<EditorMenuEditorSetting>();
    _rootDock->RegisterGui<EditorMenuFileSystemSetting>();
    _rootDock->RegisterGui<EditorPlayMenu>();

    _rootDock->RegisterGui<EditorMenuTools>(_rootDock);
}

void GameApplication::BuildSceneDock() 
{
    auto& dockSystem = _editorModule->GetDockWindowSystem();

    _sceneDock = dockSystem.RegisterDockWindow("SceneDock", _rootDock);
    
    ImGuiWindowClass imguiwindowClass;
    imguiwindowClass.ClassId                = ImHashStr("SceneDockID");
    imguiwindowClass.DockingAllowUnclassed  = false;
    imguiwindowClass.DockingAlwaysTabBar    = true;

    int imguiWindowFlag   = ImGuiWindowFlags_MenuBar;
    int imguiDockNodeFlag =
        ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;
    int editorToolFlag    = EditorTool::EDITORTOOL_FLAGS_NO_CLOSE_BUTTON;

    _sceneDock->SetWindowClass(imguiwindowClass);
    _sceneDock->SetEditorToolFlags(editorToolFlag);
    _sceneDock->SetImGuiWindowFlag(imguiWindowFlag);
    _sceneDock->SetImGuiDockNodeFlag(imguiDockNodeFlag);
    _sceneDock->SetDockLayout(ImGuiDir_Up);

    _sceneDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Right, 0.25f);
    _sceneDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Down, 0.40f);
    _sceneDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Left, 0.30f);
    _sceneDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Up, 0.50f);

    _sceneDock->RegisterGui<EditorDebugTool>();
    _sceneDock->RegisterGui<EditorHierarchyTool>();
    _sceneDock->RegisterGui<EditorInspectorTool>();
    _sceneDock->RegisterGui<EditorSceneTool>();
    _sceneDock->RegisterGui<EditorLogsTool>();
    _sceneDock->RegisterGui<EditorCommandTool>();
    _sceneDock->RegisterGui<EditorAssetBrowserTool>();

    _sceneDock->RegisterGui<EditorMenuTools>(_sceneDock);
    _sceneDock->RegisterGui<EditorSceneMenuScenes>();
}

void GameApplication::BuildModelDock()
{
    auto& dockSystem = _editorModule->GetDockWindowSystem();

    _modelDock       = dockSystem.RegisterDockWindow("ModelDock", _rootDock);

    ImGuiWindowClass imguiwindowClass;
    imguiwindowClass.ClassId               = ImHashStr("ModelDockID");
    imguiwindowClass.DockingAllowUnclassed = false;
    imguiwindowClass.DockingAlwaysTabBar   = true;

    int imguiWindowFlag                    = ImGuiWindowFlags_MenuBar;
    int dockWindowFlag                     = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;

    _modelDock->SetWindowClass(imguiwindowClass);
    _modelDock->SetImGuiWindowFlag(imguiWindowFlag);
    _modelDock->SetImGuiDockNodeFlag(dockWindowFlag);
    
    _modelDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Right, 0.25f);
    _modelDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Down, 0.40f);
    _modelDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Left, 0.30f);
    _modelDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Up, 0.50f);

    _modelDock->RegisterGui<EditorModelTool>();
    _modelDock->RegisterGui<EditorModelDetails>();
    _modelDock->RegisterGui<EditorModelHierarchy>();

    // Menu
    _modelDock->RegisterGui<EditorModelMenu>();
    _modelDock->RegisterGui<EditorMenuTools>(_modelDock);
}