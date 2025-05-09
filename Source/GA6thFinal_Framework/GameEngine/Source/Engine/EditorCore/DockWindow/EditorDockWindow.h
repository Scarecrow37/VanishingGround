#pragma once

using EditorDockWindowFlags = UINT64; // EditorDockWindow::Flags

class EditorDockWindow : public EditorGui
{
public:
    enum Flags
    {
        DOCKWINDOW_FLAGS_NONE       = 0,
        DOCKWINDOW_FLAGS_FULLSCREEN = 1 << 0,
        DOCKWINDOW_FLAGS_PADDING    = 1 << 1,
    };

private:
    /* internal */
    struct DockSplitInfo
    {
        ImGuiID     DockID    = 0;
        ImGuiDir    Direction = ImGuiDir::ImGuiDir_None;
        float       Ratio = 0.0f;
    };

public:
    EditorDockWindow()          = default;
    virtual ~EditorDockWindow() = default;

public:
    // EditorGui을(를) 통해 상속됨
    void OnTickGui() override;
    void OnStartGui() override;
    void OnDrawGui() override;
    void OnEndGui() override;

private:
    /* 최초로 에디터를 킬 경우 초기 툴의 DockSpace 공간 지정 */
    void InitDockLayout();
    /* DockSpace를 현재의 Flag, Style 기반으로 등록 */
    void SubmitDockSpace();
    /* DockWindow Flag 업데이트 (SubmitDockSpace 이전에 호출해야 함) */
    void UpdateFlag();
    /* DockSytle Push (SubmitDockSpace 이전에 호출해야 함) */
    void PushDockStyle();
    /* DockSytle Pop (SubmitDockSpace 이후에 호출해야 함) */
    void PopDockStyle();

public:
    /*  */
    void SetOptionFlags(EditorDockWindowFlags flags);
    /* 도킹 레이아웃 노드를 추가합니다. */
    void AddDockLayoutNode(ImGuiDir direction, float ratio);

private:
    UINT _flags; /* 도킹 윈도우 플래그 값 */

    ImGuiDockNodeFlags          _userImGuiDockFlags;    /* 사용자 DockSpace 플래그 값 */
    ImGuiWindowFlags            _userImGuiWindowFlags;  /* 사용자 DockWindow 플래그 값 */
    ImGuiID                     _dockSplitMainID;       /* 메인 도킹영역에 대한 ID값 */
    std::vector<DockSplitInfo>  _dockSplitLayoutID;     /* 도킹 영역에 대한 ID값 */

private:
    ImGuiWindowClass   _imGuiWindowClass; /* 윈도우 클래스 */
    ImGuiDockNodeFlags _imGuiDockFlags;   /* 최종 DockSpace 플래그 값 */
    ImGuiWindowFlags   _imGuiWindowFlags; /* 최종 DockWindow 플래그 값 */

public:
    /* Dock에 대한 플래그 설정 */
    inline void SetDockFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags = flags; }
    inline void AddDockFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags |= flags; }
    inline void RemoveDockFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags &= ~flags; }
    inline auto GetDockFlag() { return _userImGuiDockFlags; }

    /* 도킹 윈도우에 대한 플래그 설정 */
    inline void SetWindowFlag(ImGuiWindowFlags flags) { _userImGuiWindowFlags = flags; }
    inline void AddWindowFlag(ImGuiWindowFlags flags) { _userImGuiWindowFlags |= flags; }
    inline void RemoveWindowFlag(ImGuiWindowFlags flags) { _userImGuiWindowFlags &= ~flags; }
    inline auto GetWindowFlag() { return _userImGuiWindowFlags; }
};
