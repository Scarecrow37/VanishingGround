#include "pch.h"
using namespace Global;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return true;
    }

    if (App)
    {
        for (auto& handle : App->_messageHandleList)
        {
            if (handle._handle(hwnd, msg, wParam, lParam))
            {
                return true;
            }
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool Application::IsEditor()
{
    return Global::editorModule != nullptr;
}

Application::Application()
{
    if (App)
    {
        assert(!"Application은 하나만 존재 가능합니다.");
        _isQuit = true;
        return;
    }
    App = this;

    //필수 모듈들
    AddModule<EngineCoresModule>();

    _imguiDX12Module = AddModule<ImGuiDX12Module>();
    _filesystemModule = AddModule<FileSystemModule>();
}

void Application::Initialize(HINSTANCE hInstance)
{
    //로케일 설정
    std::setlocale(LC_ALL, ".UTF-8");

    //HINSTANCE 등록
    _hInstance = hInstance;

    //윈도우 생성
    CreateWindowClient();

    //모듈 초기화
    InitModules();
}

void Application::UnInitialize()
{
    //모듈 해제
    UnInitModules();
}

void Application::Run()
{
    while (!_isQuit)
    {
        if (PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE))
        {
            if (_msg.message == WM_QUIT)
            {
                _isQuit = true;
                break;
            }
            TranslateMessage(&_msg); 
            DispatchMessage(&_msg);
        }
        else
        {
            ETimeSystem::Engine::TimeSystemUpdate();
            float deltaTime = engineCore->Time.DeltaTime();

            _imguiDX12Module->ImguiBegin();
            {
                if (UmApplication.IsEditor())
                {
                    _filesystemModule->Update();
                    Global::editorModule->Update();
                }

                // AnimationUpdate
                Global::engineCore->Graphics.UpdateAnimation(deltaTime);
                
                ESceneManager::Engine::SceneUpdate();
                // CameraUpdate, RenderQueueUpdate, Render
                Global::engineCore->Graphics.Update();
                _imguiDX12Module->ImguiEnd();
                Global::engineCore->Graphics.Render();
            }
        }
    }
}

void Application::SetStyleToWindowed()
{
    _windowStyleEX = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
}

void Application::SetStyleToBorderlessWindowed()
{
    _windowStyleEX = WS_POPUP;
}

void Application::SetOptimalScreenSize()
{
    _clientSize = { 0, 0 };
}

void Application::CreateWindowClient()
{
    // 윈도우 클래스 구조체 초기화
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = _hInstance;   // 인스턴스 핸들
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = _winClassName;

    // 윈도우 클래스 등록
    if (!RegisterClassEx(&wc))
    {
        int error = GetLastError();
        wchar_t errorStr[25]{};
        swprintf_s(errorStr, L"윈도우 클래스 등록 실패. 에러 : %d", error);

        MessageBox(NULL, errorStr, L"에러", MB_OK | MB_ICONERROR);
        _isQuit = true;
        return;
    }

    ClientHelper::ClampScreenMaxSize(_clientSize);
    RECT clientRect = { 0, 0, _clientSize.cx, _clientSize.cy };
    AdjustWindowRect(&clientRect, _windowStyleEX, FALSE);
    SIZE windowSize{ clientRect.right - clientRect.left , clientRect.bottom - clientRect.top };
    SIZE windowClientOffset{ windowSize.cx - _clientSize.cx, windowSize.cy - _clientSize.cy };

    _clientSize.cx = windowSize.cx - windowClientOffset.cx;
    _clientSize.cy = windowSize.cy - windowClientOffset.cy;

    // 윈도우 생성
    _hWnd = CreateWindowEx(
        0,
        _winClassName,
        _windowName,
        _windowStyleEX,
        0, 0,
        windowSize.cx, windowSize.cy,
        NULL, NULL, _hInstance, NULL
    );

    if (!_hWnd)
    {
        int error = GetLastError();
        wchar_t errorStr[25]{};

        swprintf_s(errorStr, L"윈도우 생성 실패. 에러 : %d", error);
        MessageBox(_hWnd, errorStr, L"에러", MB_OK | MB_ICONERROR);
        _isQuit = true;
        return;
    }

    //윈도우 표시
    ShowWindow(_hWnd, 10);
    UpdateWindow(_hWnd);
    ClientHelper::WinToScreenCenter(_hWnd);
}


void ClientHelper::WinToScreenCenter(HWND hwnd)
{
    int x, y, width, height;
    RECT rtDesk, rtWindow;
    GetWindowRect(GetDesktopWindow(), &rtDesk);
    GetWindowRect(hwnd, &rtWindow);
    width = rtWindow.right - rtWindow.left;
    height = rtWindow.bottom - rtWindow.top;
    x = (rtDesk.right - width) / 2;
    y = (rtDesk.bottom - height) / 2;
    MoveWindow(hwnd, x, y, width, height, FALSE);
}

void ClientHelper::ClampScreenMaxSize(SIZE& size)
{
    const SIZE maxScreenSize = { GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    if (size.cx <= 0 || 0 >= size.cy ||
        size.cx > maxScreenSize.cx || maxScreenSize.cy < size.cy)
    {
        size = maxScreenSize;
    }
}

void Application::InitModules()
{
    for (auto iter = _appModuleList.begin(); iter != _appModuleList.end(); ++iter)
    {
        auto& appModule = *iter;
        appModule->PreInitialize();
    }
    for (auto iter = _appModuleList.begin(); iter != _appModuleList.end(); ++iter)
    {
        auto& appModule = *iter;
        appModule->ModuleInitialize();
    }
    _moduleInit = true;
}

void Application::UnInitModules()
{
    for (auto iter = _appModuleList.rbegin(); iter != _appModuleList.rend(); ++iter)
    {
        auto& appModule = *iter;
        appModule->PreUnInitialize();
    }
    for (auto iter = _appModuleList.rbegin(); iter != _appModuleList.rend(); ++iter)
    {
        auto& appModule = *iter;
        appModule->ModuleUnInitialize();
    }
    _appModuleList.clear();
}

void Application::MainEntry::Initialize(HINSTANCE hInstance) 
{
    Application::App->Initialize(hInstance);
}

void Application::MainEntry::UnInitialize() 
{
    Application::App->UnInitialize();
}

void Application::MainEntry::Run() 
{
    Application::App->Run();
}
