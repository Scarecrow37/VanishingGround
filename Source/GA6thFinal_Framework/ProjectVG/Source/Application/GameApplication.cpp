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
    BuildAssetDock();
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

    ImGuiWindowClass windowClass;
    windowClass.ClassId               = ImHashStr("RootDockID"); // 윈도우 ID값 (그냥 대충 ImHashStr을 사용하여 생성)
    windowClass.DockingAllowUnclassed = false; // 허용되지 않은 윈도우의 도킹을 허용할 것인가
    windowClass.DockingAlwaysTabBar   = false; // 도킹 탭바를 항상 표시할 것인가

    int windowFlag = 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;

    int dockNodeFlag = 
        ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;

    _rootDock->SetWindowClass(windowClass);
    _rootDock->SetImGuiWindowFlag(windowFlag);
    _rootDock->SetImGuiDockNodeFlag(dockNodeFlag);
    _rootDock->SetDockWindowFlags(EditorDockWindow::DOCKWINDOW_FLAGS_FULLSCREEN);

    //_rootDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Up, 0.70f);
    //_rootDock->CreateDockLayoutNode(ImGuiDir::ImGuiDir_Down, 0.30f);

    _rootDock->RegisterGui<EditorMenuProjectRoot>();
    _rootDock->RegisterGui<EditorMenuScriptBuilder>();
    _rootDock->RegisterGui<EditorBuildSettingMenu>(); 
    _rootDock->RegisterGui<EditorMenuDebug>();
    _rootDock->RegisterGui<EditorMenuStyleEditor>();
    _rootDock->RegisterGui<EditorMenuFileSystemSetting>();
}

void GameApplication::BuildSceneDock() 
{
    auto& dockSystem = _editorModule->GetDockWindowSystem();

    _sceneDock = dockSystem.RegisterDockWindow("SceneDock", _rootDock);
    
    ImGuiWindowClass windowClass;
    windowClass.ClassId               = ImHashStr("SceneDockID");
    windowClass.DockingAllowUnclassed = false;
    windowClass.DockingAlwaysTabBar   = true;

    int windowFlag   = ImGuiWindowFlags_MenuBar;
    int dockNodeFlag = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton;

    _sceneDock->SetWindowClass(windowClass);
    _sceneDock->SetImGuiWindowFlag(windowFlag);
    _sceneDock->SetImGuiDockNodeFlag(dockNodeFlag);
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
    _sceneDock->RegisterGui<EditorModelTool>();
    _sceneDock->RegisterGui<EditorAssetBrowserTool>();

    _sceneDock->RegisterGui<EditorMenuTools>(_sceneDock);
    _sceneDock->RegisterGui<EditorSceneMenuScenes>();
}

void GameApplication::BuildAssetDock() 
{
    auto& dockSystem = _editorModule->GetDockWindowSystem();

    _assetDock = dockSystem.RegisterDockWindow("AssetDock", _rootDock);

    ImGuiWindowClass windowClass;
    windowClass.ClassId               = ImHashStr("AssetDockID");
    windowClass.DockingAllowUnclassed = false;
    windowClass.DockingAlwaysTabBar   = true;

    int dockNodeFlag = ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton |
                       ImGuiDockNodeFlags_NoTabBar;

    _assetDock->SetWindowClass(windowClass);
    _assetDock->SetImGuiDockNodeFlag(dockNodeFlag);
    _assetDock->SetDockLayout(ImGuiDir_Down);
}
