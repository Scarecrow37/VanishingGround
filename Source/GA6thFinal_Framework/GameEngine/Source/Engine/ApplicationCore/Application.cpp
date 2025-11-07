#include "pch.h"
#include "minidumpapiset.h"
using namespace Global;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

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

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return true;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

#ifdef _UMEDITOR
void Application::Quit(std::source_location location) 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, u8"게임 종료 요청이 호출되었습니다.", location);
}
#endif //  _UMEDITOR

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
    _audioModule      = AddModule<AudioModule>();
    AddModule<GraphicsModule>();
}

bool Application::AppMessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_SIZE)
    {
        if (nullptr != UmCore.get())
        {
            Application& app = UmApplication;
            app._clientSize.cx = LOWORD(lParam); 
            app._clientSize.cy = HIWORD(lParam);
            if (app._clientSize.cx > 0.f && app._clientSize.cy > 0.f)
            {
                UmCore->Graphics.OnResize(app._clientSize.cx, app._clientSize.cy);
            }      
            return true;
        }        
    }
    return false;
}

void Application::Initialize(HINSTANCE hInstance)
{
    //로케일 설정
    std::setlocale(LC_ALL, ".UTF-8");

    //HINSTANCE 등록
    _hInstance = hInstance;

    //윈도우 생성
    CreateWindowClient();

    //CoInitialize
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        __debugbreak();
    }

    //모듈 초기화
    InitModules();

    //기본 메시지 핸들러 등록
    MessageHandler handle(AppMessageHandler, 0);
    AddMessageHandler(handle);

    //게임 모드 체크
    if constexpr (false == Application::IsEditor())
    {
        UmFileSystem.LoadGameDirectory();
    }

    //Factory 초기화
    UmFactoryRegister.InvokeRegister();

    //초기화 완료
    OnStartupComplete();
}

void Application::UnInitialize()
{
    //모듈 해제
    UnInitModules();

    //CoUninitialize
    CoUninitialize();

    //해제 완료
    OnShutdownComplete();
}

void Application::Run()
{
#ifndef _UMEDITOR
    try
#endif
    {
        while (!_isQuit)
        {
            if (PeekMessage(&_msg, NULL, 0, 0, PM_REMOVE))
            {
                if (_msg.message == WM_QUIT)
                {
                    Quit();
                    break;
                }
                TranslateMessage(&_msg);
                DispatchMessage(&_msg);
            }
            else
            {
                // Time System Update
                ETimeSystem::Engine::TimeSystemUpdate();
                float deltaTime = engineCore->Time.DeltaTime();

                // Imgui begin
                _imguiDX12Module->ImguiBegin();

                // Debugger Window
                Global::engineCore->DebuggerWindow.Update();

                // Editor Update
                if constexpr (true == Application::IsEditor())
                {
                    _filesystemModule->Update();
                    Global::editorModule->Update();
                    Global::engineCore->UpdateIsPlay(); // 에디터 업데이트 이후 플레이 여부 갱신 해야함
                }

                // AnimationUpdate
                Global::engineCore->Graphics.UpdateAnimation(deltaTime);

                // Scene Logic Update
                ESceneManager::Engine::SceneUpdate();

                // User Interface Update
                // TODO: Erase Magic Number Resolution
                Global::engineCore->UserInterface.Update({1920, 1080});

                // CameraUpdate, RenderQueueUpdate, Render
                Global::engineCore->Graphics.Update(deltaTime);
                Global::engineCore->Graphics.Render();

                // Scene Final Update
                ESceneManager::Engine::SceneFinalUpdate();

                _imguiDX12Module->ImguiEnd();

                Global::engineCore->Graphics.Flip();
            }
        }
    }
#ifndef _UMEDITOR
    catch (const std::exception& ex)
    {
        UmLogger.Log(LogLevel::LEVEL_FATAL, ex.what());
        CreateMiniDump(nullptr, MiniDumpWithFullMemory | MiniDumpWithThreadInfo);
    }
#endif
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

    if (NULL != _winClassIconPath)
    {
        wc.hIcon = (HICON)LoadImage(NULL, _winClassIconPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    }

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

