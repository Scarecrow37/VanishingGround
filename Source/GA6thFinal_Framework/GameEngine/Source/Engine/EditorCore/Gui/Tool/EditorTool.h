#pragma once

class EditorDockWindow;

/*
도킹이 가능한 에디터 윈도우를 생성할 수 있는 객체
*/
class EditorTool : public EditorGui
{
    friend class EditorDockWindow;
    using Super = EditorGui;

public:
    enum Flags
    {
        EDITORTOOL_FLAGS_NONE               = 0,
        EDITORTOOL_FLAGS_ALWAYS_FRAME       = 1 << 1,   // 항상 프레임을 열고 닫음
        EDITORTOOL_FLAGS_NO_CLOSE_BUTTON    = 1 << 2,   // 닫기 버튼을 없앰
        EDITORTOOL_FLAGS_NO_PADDING         = 1 << 3,   // 패딩을 없앰
        // [Internal] Flags
        EDITORTOOL_FLAGS_IS_DOCKWINDOW      = 1 << 4,   // 도킹 윈도우 여부
    };

public:
    EditorTool();
    virtual ~EditorTool();

public:
    virtual void OnTickGui() override {}
    virtual void OnStartGui() override {};
    virtual void OnDrawGui() override final;
    virtual void OnEndGui() override {};

private:
    /// <summary>
    /// ImGui::Begin 호출 직전에 호출됩니다.
    /// </summary>
    virtual void OnPreFrameBegin();

    /// <summary>
    /// ImGui::Begin 호출 직후에 호출됩니다.
    /// </summary>
    virtual void OnPostFrameBegin();

    /// <summary>
    /// Gui의 프레임이 클리핑이 될 때 1회 호출됩니다.
    /// </summary>
    virtual void OnFrameClipped();

    /// <summary>
    /// ImGui::End 호출 직후에 호출됩니다.
    /// </summary>
    virtual void OnFrameEnd();

    /* 프레임이 포커싱 될 때 호출 (OnPostFrameBegin 후에 호출) */

    /// <summary>
    /// 프레임이 포커싱 상태에 진입되었을 때 1회 호출됩니다.
    /// </summary>
    virtual void OnFrameFocusEnter();

    /// <summary>
    /// 프레임이 포커싱 상태에 있는 동안 매 프레임마다 호출됩니다.
    /// </summary>
    virtual void OnFrameFocusStay();

    /// <summary>
    /// 프레임이 포커싱 상태에서 벗어났을 때 1회 호출됩니다.
    /// </summary>
    virtual void OnFrameFocusExit();

    /// <summary>
    /// ImGui::Begin 호출 후 클리핑 테스트를 통과할 시 호출됩니다.
    /// </summary>
    virtual void OnFrameRender();

    /// <summary>
    /// 프레임 Tab의 팝업이 열렸을 때 호출됩니다.
    /// </summary>
    virtual void OnFramePopupOpened();

private:
    void PushStyle();
    void PopStyle();
    void BeginFrame();
    void EndFrame();
    void InitFrame();

    virtual void ProcessPopupFrame();
    virtual void ProcessFocusFrame();
    virtual void ProcessRenderFrame();

protected:
    REFLECT_FIELDS_BEGIN(EditorGui)
    std::string OriginLabel;
    bool IsLock = false;
    REFLECT_FIELDS_END(EditorTool)

private:
    std::string                     _label                  = "";                       // 에디터 툴 이름 (기본적으로 전역 단위의 이름 중복을 허용하지 않음. 나중엔 uuid등으로 관리할지 고민 중)
    bool                            _isDrawable             = false;                    // 해당 탭이 보일지에 대한 여부
    bool                            _isBeginningFrame       = false;                    // BeginFrame이 호출 중인지 여부
    int                             _editorToolOptionFlags  = EDITORTOOL_FLAGS_NONE;    // 옵션 플래그

    ImGuiWindowClass                _imGuiWindowClass       = {};
    std::pair<bool, ImVec2>         _size                   = {false, ImVec2(0, 0)};    // 사이즈 조정 여부와 사이즈
    std::pair<bool, ImVec2>         _pos                    = {false, ImVec2(0, 0)};    // 위치 조정 여부와 위치
    std::pair<bool, ImGuiDir>       _dockLayout             = {false, ImGuiDir_None};   // 초기 Dock영역 (초기 도킹빌드시에만 사용하고 이후엔 사용 X)
    ImGuiWindowFlags                _windowFlags            = ImGuiWindowFlags_None;    // ImGui윈도우 플래그 (ImGuiWindowFlags_NoCollapse는 항상 활성화)
    
    EditorDockWindow*               _ownerDockWindow        = nullptr;                  // 도킹 스페이스 (부모 도킹스페이스)

private:
    ImGuiWindow*                    _imguiWindow            = nullptr;  // [Internal] ImGuiWindow 클래스
    int                             _imguiSytleStackCount   = 0;        // [Internal] PushStyleVar 호출 횟수
    bool                            _isDockWindow           = false;    // [Internal] 도킹윈도우인지 여부
    bool                            _isFirstTick            = true;     // [Internal] 첫 번째 Tick인지 여부
    bool                            _isFrameDisable         = false;    // [Internal] BeginDisabled 호출 여부
    bool                            _isFrameFocused         = false;    // [Internal] 이전 FrameFocus 여부
    bool                            _isFrameDrawed          = false;    // [Internal] 이전 FrameRender 호출 여부

public:
    inline void                     SetWindowClass(const ImGuiWindowClass& windowClass) { _imGuiWindowClass = windowClass; }
    inline const ImGuiWindowClass&  GetWindowClass() const { return _imGuiWindowClass; }

    /*                              이름 설정 (기본적으로 중복을 비허용.) */
    inline void                     SetLabel(const char* label) { _label = label; }
    inline const std::string&       GetLabel() const { return _label; }

    /*                              초기 도킹 영역을 지정 */
    inline void                     SetDockLayout(ImGuiDir layout) { _dockLayout = {true, layout}; }
    inline ImGuiDir                 GetDockLayout() const { return _dockLayout.second; }

    /*                              플래그 설정 */
    inline void                     SetEditorToolFlags(int flags) { _editorToolOptionFlags = flags; }
    inline void                     AddEditorToolFlags(int flags) { _editorToolOptionFlags |= flags; }
    inline void                     RemoveEditorToolFlags(int flags) { _editorToolOptionFlags &= ~flags; }
    inline int                      GetEditorToolFlags() const { return _editorToolOptionFlags; }
    inline bool                     HasEditorToolFlags(int flags) const { return _editorToolOptionFlags & flags; }

    inline void                     SetImGuiWindowFlag(ImGuiWindowFlags flag) { _windowFlags = flag; }
    inline void                     AddImGuiWindowFlag(ImGuiWindowFlags flag) { _windowFlags |= flag; }
    inline void                     RemoveImGuiWindowFlag(ImGuiWindowFlags flag) { _windowFlags &= ~flag; }
    inline ImGuiWindowFlags         GetImGuiWindowFlag() const { return _windowFlags; }
    inline bool                     HasImGuiWindowFlag(ImGuiWindowFlags flag) const { return _windowFlags & flag; }

    /*                              툴 잠금 설정 */
    inline void                     SetLock(bool v) { ReflectFields->IsLock = v; }
    inline void                     ToggleLock() { ReflectFields->IsLock = ReflectFields->IsLock == true ? false : true; }
    inline bool                     IsLock() const { return ReflectFields->IsLock; }
   
    /*                              사이즈 조정 설정 */
    inline void                     SetSize(const ImVec2& size) { _size = {true, size}; }
    inline ImVec2                   GetSize() const { return _size.second; }

    /*                              위치 조정 설정 */
    inline void                     SetPos(const ImVec2& pos) { _pos = {true, pos}; }
    inline ImVec2                   GetPos() const { return _pos.second; }

    /*                              도킹 스페이스 설정 (부모 도킹스페이스) */
    inline void                     SetOwnerDockWindow(EditorDockWindow* dockWindow) { _ownerDockWindow = dockWindow; }
    inline EditorDockWindow*        GetOwnerDockWindow() const { return _ownerDockWindow; }

    inline ImGuiWindow*             GetImGuiWindow() const { return _imguiWindow; }
    /*                              렌더링 가능 여부 */
    inline bool                     IsDrawable() const { return _isDrawable; }
    /*                              Begin과 End 사이의 작업 중인지 여부 */
    inline bool                     IsBeginningFrame() const { return _isBeginningFrame; }
    /*                              해당 프레임이 포커싱 중인지 여부 */ 
    inline void                     SetFocusFrame() const { ImGui::SetWindowFocus(GetLabel().c_str()); }
    inline bool                     IsFocusFrame() const { return _isFrameFocused; }
    /*                              해당 객체가 도킹 윈도우인지 반환 */
    inline bool                     IsDockWindow() const { return _isDockWindow; }
};

