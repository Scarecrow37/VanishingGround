#pragma once

/// <summary>
/// 메시지 이벤트를 위한 핸들러입니다.
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

class Application
{
    friend class EngineCores;
    friend struct SafeEngineCoresPtr;
    inline static Application* App = nullptr;
    static LRESULT CALLBACK    WndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                       LPARAM lParam);
public:
    /// <summary>
    /// 앱을 종료합니다.
    /// </summary>
    inline void Quit() { _isQuit = true; }

    /// <summary>
    /// 앱 종료 여부를 확인합니다. 컴포넌트에서는 OnApplicationQuit 함수를
    /// 호출하기 때문에 사용할 필요 없습니다.
    /// </summary>
    /// <returns>isQuit 여부</returns>
    inline bool IsQuit() const { return _isQuit; }

    /// <summary>
    /// 에디터 모드 여부를 확인합니다.
    /// </summary>
    bool IsEditor();

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
    /// 메시지 핸들러를 등록합니다. Application에서 메시지 피크시 핸들러들에게
    /// 메시지를 전달해줍니다.
    /// </summary>
    /// <param name="handle :">사용할 핸들러 함수</param>
    /// <param name="messageOrder :">우선순위</param>
    inline void AddMessageHandler(const MessageHandler& handle)
    {
        _messageHandleList.emplace_back(handle);
        std::sort(App->_messageHandleList.begin(),
                  App->_messageHandleList.end(),
                  [](MessageHandler& handleA, MessageHandler& HandleB) {
                      return handleA < HandleB;
                  });
    }

public:
    Application();
    virtual ~Application() = default;

    struct MainEntry
    {
        static void Initialize(HINSTANCE hInstance);
        static void UnInitialize();
        static void Run();
    };
private:
    void Initialize(HINSTANCE hInstance);
    void UnInitialize();
    void Run();

private:
    void CreateWindowClient();
    void InitModules();
    void UnInitModules();

private:
    class ImGuiDX11Module*  _imguiDX11Module = nullptr;
    class GraphicsModule*   _graphicsModule  = nullptr;
    class FileSystemModule* _filesystemModule = nullptr;

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

protected:
    /** 클래스 스타일. Initialize() 실행전에만 적용됩니다.*/
    UINT _winClassStyle = CS_HREDRAW | CS_VREDRAW;

    /** 클라이언트 윈도우 스타일. Initialize() 실행전에만 적용됩니다. 기본값 :
     * 테두리 없는 창모드*/
    DWORD _windowStyleEX = WS_POPUP;

    /** 윈도우 클래스 구조체 이름. Initialize() 실행전에만 적용됩니다.*/
    LPCWSTR _winClassName = L"UmrealEngineClass";

    /** 윈도우 클라이언트 이름. Initialize() 실행전에만 적용됩니다.*/
    LPCWSTR _windowName = L"DemoApp";

    /** 클라이언트 사이즈. Initialize() 실행전에만 적용됩니다. 기본 값 : 화면
     * 해상도*/
    SIZE _clientSize = {0, 0};

private:
    bool      _isQuit    = false;
    HWND      _hWnd      = NULL;
    HINSTANCE _hInstance = NULL;
    MSG       _msg{};

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
