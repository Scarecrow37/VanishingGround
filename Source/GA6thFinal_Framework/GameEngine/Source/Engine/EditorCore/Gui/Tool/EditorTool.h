#pragma once

class EditorDockWindow;

/*
도킹이 가능한 에디터 윈도우를 생성할 수 있는 객체
*/
class EditorTool : public EditorGui
{
    friend class EditorModule;

public:
    enum Flags
    {
        EDITORTOOL_FLAGS_NONE               = 0,
        EDITORTOOL_FLAGS_ALWAYS_FRAME       = 1 << 2,   // 항상 프레임을 열고 닫음.
        EDITORTOOL_FLAGS_NO_PADDING         = 1 << 3,   // 패딩을 없앰
    };

public:
    EditorTool() = default;
    virtual ~EditorTool() = default;
public:
    virtual void OnTickGui() override {}
    virtual void OnStartGui() override {};
    virtual void OnDrawGui() override;
    virtual void OnEndGui() override {};

private:
    /* Begin 호출 전에 호출 */
    virtual void OnPreFrameBegin();

    /* Begin 호출 직후 호출 */
    virtual void OnPostFrameBegin();
    
    /* Begin 호출 후 클리핑 테스트를 통과한 후 호출 */
    virtual void OnFrameRender();
    virtual void OnFrameClipped();

    /* End 호출 후에 호출 */
    virtual void OnFrameEnd();

    /* 프레임이 포커싱 될 때 호출 (OnPostFrameBegin 후에 호출) */
    virtual void OnFrameFocusEnter();
    virtual void OnFrameFocusStay();
    virtual void OnFrameFocusExit();

    /* Popup창 호출 성공 시 호출 (OnPreFrameBegin 전에 호출) */
    virtual void OnFramePopupOpened();

    // 추가 할 것
    //virtual void OnFrameClosed();
    //
    //virtual void OnFrameOpened();
    //
    //virtual void OnFrameResized();

private:
    void PushStyle();
    void PopStyle();
    void BeginFrame();
    void EndFrame();
    void InitFrame();

    void ProcessPopupFrame();
    void ProcessFocusFrame();
    void ProcessRenderFrame();

private:
    std::string                     _label                  = "";                       // 에디터 툴 이름 (기본적으로 전역 단위의 이름 중복을 허용하지 않음. 나중엔 uuid등으로 관리할지 고민 중)
    bool                            _isLock                 = false;                    // 해당 탭에 대한 입력을 막을지에 대한 여부
    bool                            _isDrawable              = false;                   // 해당 탭이 보일지에 대한 여부
    bool                            _isBeginningFrame       = false;                    // BeginFrame이 호출 중인지 여부
    bool                            _isFirstTick            = true;                     // 첫 번째 Tick인지 여부
    int                             _editorToolOptionFlags  = EDITORTOOL_FLAGS_NONE;    // 옵션 플래그

    ImGuiWindowClass                _imGuiWindowClass       = {};
    std::pair<bool, ImVec2>         _size                   = {false, ImVec2(0, 0)};    // 사이즈 조정 여부와 사이즈
    std::pair<bool, ImVec2>         _pos                    = {false, ImVec2(0, 0)};    // 위치 조정 여부와 위치
    std::pair<bool, ImGuiDir>       _dockLayout             = {false, ImGuiDir_None};   // 초기 Dock영역 (초기 도킹빌드시에만 사용하고 이후엔 사용 X)
    ImGuiWindowFlags                _windowFlags            = ImGuiWindowFlags_None;    // ImGui윈도우 플래그 (ImGuiWindowFlags_NoCollapse는 항상 활성화)
    
    EditorDockWindow*               _ownerDockWindow        = nullptr;                  // 도킹 스페이스 (부모 도킹스페이스)
    std::vector<EditorDockWindow*>  _childDockWindowList;                               // 도킹 스페이스 리스트 (자식 도킹스페이스)

private:
    ImGuiWindow*                    _imguiWindow            = nullptr;  // [Internal] ImGuiWindow 클래스
    int                             _imguiSytleStackCount   = 0;        // [Internal] PushStyleVar 호출 횟수
    bool                            _isFrameDisable         = false;    // [Internal] BeginDisabled 호출 여부
    bool                            _isFrameFocused         = false;    // [Internal] 이전 FrameFocus 여부
    bool                            _isFrameDrawed          = false;    // [Internal] 이전 FrameRender 호출 여부

public:
    inline void         SetWindowClass(const ImGuiWindowClass& windowClass) { _imGuiWindowClass = windowClass; }
    inline const auto&  GetWindowClass() { return _imGuiWindowClass; }

    /*                  이름 설정 (기본적으로 중복을 비허용.) */
    inline void         SetLabel(const std::string& label) { _label = label; }
    inline const auto&  GetLabel() { return _label; }

    /*                  초기 도킹 영역을 지정 */
    inline void         SetDockLayout(ImGuiDir layout) { _dockLayout = {true, layout}; }
    inline ImGuiDir     GetDockLayout() { return _dockLayout.second; }

    /*                  플래그 설정 */
    inline void         SetEditorToolFlags(UINT flags) { _editorToolOptionFlags = flags; }
    inline void         AddEditorToolFlags(UINT flags) { _editorToolOptionFlags |= flags; }
    inline void         RemoveEditorToolFlags(UINT flags) { _editorToolOptionFlags &= ~flags; }
    inline int          GetEditorToolFlags() { return _editorToolOptionFlags; }
    inline bool         HasEditorToolFlags(UINT flags) { return _editorToolOptionFlags & flags; }

    inline void         SetImGuiWindowFlag(ImGuiWindowFlags flag) { _windowFlags = flag; }
    inline void         AddImGuiWindowFlag(ImGuiWindowFlags flag) { _windowFlags |= flag; }
    inline void         RemoveImGuiWindowFlag(ImGuiWindowFlags flag) { _windowFlags &= ~flag; }
    inline auto         GetImGuiWindowFlag() { return _windowFlags; }
    inline bool         HasImGuiWindowFlag(ImGuiWindowFlags flag) { return _windowFlags & flag; }

    /*                  툴 잠금 설정 */
    inline void         SetLock(bool v) { _isLock = v; }
    inline bool         IsLock() { return _isLock; }
    inline void         ToggleLock() { _isLock = _isLock == true ? false : true; }

    /*                  사이즈 조정 설정 */
    inline void         SetSize(const ImVec2& size) { _size = {true, size}; }
    inline ImVec2       GetSize() { return _size.second; }

    /*                  위치 조정 설정 */
    inline void         SetPos(const ImVec2& pos) { _pos = {true, pos}; }
    inline ImVec2       GetPos() { return _pos.second; }

    /*                  도킹 스페이스 설정 (부모 도킹스페이스) */
    inline void         SetOwnerDockWindow(EditorDockWindow* dockWindow) { _ownerDockWindow = dockWindow; }
    inline auto*        GetOwnerDockWindow() { return _ownerDockWindow; }

    inline auto*        GetImGuiWindow() { return _imguiWindow; }
    /*                  렌더링 가능 여부 */
    inline bool         IsDrawable() { return _isDrawable; }
    /*                  Begin과 End 사이의 작업 중인지 여부 */
    inline bool         IsBeginningFrame() { return _isBeginningFrame; }
};

