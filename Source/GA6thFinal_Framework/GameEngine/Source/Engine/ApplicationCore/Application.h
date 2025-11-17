#pragma once

/// <summary>
/// 메시지 이벤트를 위한 핸들러입니다. 등록된 핸들러들은 Order 순에 따라 정렬되어 순서대로 메시지를 Pump합니다.
/// </summary>
struct MessageHandler
{
    friend class Application;
    using Handle = 
        std::function<bool(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)>;
    MessageHandler(const Handle& handle, long messageOrder)
        : _handle(handle), _messageOrder(messageOrder)
    {
    }
    MessageHandler(const MessageHandler& rhs)
        : _handle(rhs._handle), _messageOrder(rhs._messageOrder)
    {
    }
    bool operator<(const MessageHandler& rhs) const
    {
        return this->_messageOrder < rhs._messageOrder;
    }

private:
    Handle _handle;
    long   _messageOrder;
};


/*
* 윈도우 애플리케이션을 관리하는 클래스입니다. 프로젝트는 이 클래스를 상속받아 구성해야 합니다.
*/
class Application
{
public:
    Application();
    virtual ~Application() = default;

protected:
    /*애플리케이션의 Initialize 끝난 후 호출되는 함수입니다.*/
    virtual void OnStartupComplete() {};
    /*애플리케이션의 UnInitialize 끝난 후 호출되는 함수입니다.*/
    virtual void OnShutdownComplete() {};

private:
    friend class EngineCores;
    friend struct SafeEngineCoresPtr;
    inline static Application* App = nullptr;
    static LRESULT CALLBACK    WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                       LPARAM lParam);
public:
#ifdef _UMEDITOR
    void Quit(std::source_location location = std::source_location::current());
#else
    /// <summary>
    /// 앱을 종료합니다.
    /// </summary>
    inline void Quit() { _isQuit = true; }
#endif // _UMEDITOR

    /// <summary>
    /// 앱 종료 여부를 반환합니다. 컴포넌트에서는 OnApplicationQuit 함수를 호출하기 때문에 사용할 필요 없습니다.
    /// </summary>
    /// <returns>isQuit 여부</returns>
    inline bool IsQuit() const { return _isQuit; }

    /// <summary>
    /// 에디터 빌드 여부를 반환해줍니다.
    /// </summary>
    inline static constexpr bool IsEditor() 
    { 
        return IS_EDITOR; 
    }

    /// <summary>
    /// 현재 실행중인 클라이언트의 HINSTANCE를 반환합니다.
    /// </summary>
    /// <returns>HINSTANCE</returns>
    inline HINSTANCE GetHInstance() const { return _hInstance; }

    /// <summary>
    /// 현재 실행중인 클라이언트의 HWND를 반환합니다.
    /// </summary>
    /// <returns>HWND</returns>
    inline HWND GetHwnd() const { return _hWnd; }

    /// <summary>
    /// 메시지 핸들러를 등록합니다. Application에서 PeekMessage시 핸들러들에게메시지를 전달해줍니다.
    /// </summary>
    /// <param name="handle :">사용할 핸들러 함수</param>
    /// <param name="messageOrder :">우선순위</param>
    inline void AddMessageHandler(const MessageHandler& handle)
    {
        _messageHandleList.emplace_back(handle);
        std::sort(App->_messageHandleList.begin(),
                  App->_messageHandleList.end(),
                  [](MessageHandler& handleA, MessageHandler& HandleB) 
                  {
                      return handleA < HandleB;
                  });
    }

    /// <summary>
    /// 클라이언트 실제 영역을 반환합니다.
    /// </summary>
    /// <returns></returns>
    inline const SIZE& GetClientSize()
    { 
        return _clientSize;
    }

    inline void HideCursor(bool isHide)
    {
        if (isHide)
        {
            _hideMouseCursor = true;
            Application::ShowMouseCursor(false);
        }
        else
        {
            _hideMouseCursor = false;
            Application::ShowMouseCursor(true);
        }
    }

public:
    /*엔진을 사용하는 프로젝트의 MainEntry에서 사용해야합니다.*/
    struct MainEntry
    {
        /*앱을 초기화합니다.*/
        static void Initialize(HINSTANCE hInstance);

        /*메인 루프를 실행합니다.*/
        static void Run();

        /*앱을 정리합니다.*/
        static void UnInitialize();
    };

private:
    /*메시지 펌프를 사용할 핸들러를 등록합니다.*/
    static bool AppMessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    /*앱을 초기화합니다.*/
    void Initialize(HINSTANCE hInstance);

    /*메인 루프를 실행합니다.*/
    void Run();

    /*앱을 정리합니다.*/
    void UnInitialize();

private:
    /*윈도우 클래스 초기화 및 윈도우 클라이언트를 생성합니다.*/
    void CreateWindowClient();

    /// <summary>
    /// Application에 등록된 모듈들의 초기화 함수들을 실행합니다.
    /// 등록된 순서대로 모든 모듈의 PreInit을 호출 한 뒤 ModuleInit을 호출합니다.
    /// </summary>
    void InitModules();

    /// <summary>
    /// Application에 등록된 모듈들의 정리 함수들을 실행합니다.
    /// 등록된 순서의 역으로 PreUnInit을 호출 한 뒤 ModuleUnInit을 호출합니다.
    /// </summary>
    void UnInitModules();

private:
    class ImGuiDX12Module*  _imguiDX12Module  = nullptr;
    class FileSystemModule* _filesystemModule = nullptr;
    class AudioModule*      _audioModule      = nullptr;

protected:
    /// <summary>
    /// 모듈을 등록할때 사용합니다.
    /// Application을 상속받은 클래스의 생성자에서 호출해야지 정상적인 초기화가
    /// 이루어집니다.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template <typename T>
    T* AddModule()
    {
        // 모듈 상속받은 클래스만 등록 가능.
        static_assert(std::is_base_of_v<IAppModule, T>, "T is not a module.");

        // 중복 모듈 방지
        auto [iter, result] = _moduleSet.insert(typeid(T).name());
        if (!result)
        {
            assert(!"모듈 중복 추가는 불가능 합니다.");
            return nullptr;
        }
        if (_moduleInit)
        {
            assert(!"Application 실행중 모듈 추가는 불가능 합니다.");
            return nullptr;
        }

        _appModuleList.emplace_back(new T);
        return static_cast<T*>(_appModuleList.back().get());
    }

    /*윈도우 스타일을 창모드로*/
    void SetStyleToWindowed();

    /*윈도우 스타일을 테두리 없는 창모드로*/
    void SetStyleToBorderlessWindowed();

    /*클라이언트 크기를 모니터 해상도로*/
    void SetOptimalScreenSize();

    /*마우스 커서를 숨기거나 표시합니다.*/
    static void ShowMouseCursor(bool show);

protected:
    /** 클래스 스타일. Initialize() 실행전에만 적용됩니다.*/
    UINT _winClassStyle = CS_HREDRAW | CS_VREDRAW;

    /** 클라이언트 윈도우 스타일. Initialize() 실행전에만 적용됩니다. 기본값 : 테두리 없는 창모드*/
    DWORD _windowStyleEX = WS_POPUP;

    /** 윈도우 클래스 구조체 이름. Initialize() 실행전에만 적용됩니다.*/
    LPCWSTR _winClassName = L"UmrealEngineClass";

    /** 윈도우 클래스 아이콘 경로. Initialize() 실행전에만 적용됩니다.*/
    LPCWSTR _winClassIconPath = NULL;

    /** 윈도우 클라이언트 이름. Initialize() 실행전에만 적용됩니다.*/
    LPCWSTR _windowName = L"Umreal Engine";

    /** 클라이언트 사이즈. Initialize() 실행전에만 적용됩니다. 기본 값 : 화면
     * 해상도*/
    SIZE _clientSize = {0, 0};

private:
    bool      _isQuit    = false;
    HWND      _hWnd      = NULL;
    HINSTANCE _hInstance = NULL;
    MSG       _msg{};
    bool      _hideMouseCursor = false;

    bool                                     _moduleInit = false;
    std::unordered_set<std::string>          _moduleSet;
    std::vector<std::unique_ptr<IAppModule>> _appModuleList;
    std::vector<MessageHandler>              _messageHandleList;

};

struct ClientHelper
{
    /**클라이언트를 화면 가운데로 위치 시킵니다.*/
    static void WinToScreenCenter(HWND hwnd);

    /**클라이언트 크기를 현재 화면 해상도 설정으로 Clamp 하는 함수.*/
    static void ClampScreenMaxSize(SIZE& size);
};
