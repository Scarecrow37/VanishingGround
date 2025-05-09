#pragma once

class EditorDockSpace;

/*
도킹이 가능한 에디터 윈도우를 생성할 수 있는 객체
*/
class EditorTool : public EditorGui
{
    friend class EditorModule;
public:
    EditorTool() = default;
    virtual ~EditorTool() = default;
private:
    virtual void OnTickGui() override {}
    virtual void OnStartGui() override {};
    virtual void OnDrawGui() override;
    virtual void OnEndGui() override {};

private:
    /* Begin 호출 전에 호출 */
    virtual void OnPreFrame();

    /* Begin 호출 시 호출 */
    virtual void OnFrame();

    /* End 호출 후에 호출 */
    virtual void OnPostFrame();

    /* 프레임이 포커싱 될 때 호출 (Begin 후에 호출) */
    virtual void OnFocus();

    /* PopUp창 호출 성공 시 호출 (Begin 후에 호출) */
    virtual void OnPopup();

public:
    /*                      초기 도킹 영역을 지정 */
    inline void             SetDockLayout(DockLayout layout) { _dockLayout = layout; }
    inline DockLayout       GetDockLayout() { return _dockLayout; }
    /*                      플래그 설정 */
    inline void             SetWindowFlag(ImGuiWindowFlags flag) { _windowFlags = flag; }
    inline void             AddWindowFlag(ImGuiWindowFlags flag) { _windowFlags |= flag; }
    inline ImGuiWindowFlags GetWindowFlag() { return _windowFlags; }
    /*                      툴 잠금 설정 */
    inline void             SetLock(bool v) { _isLock = v; }
    inline bool             IsLock() { return _isLock; }
    inline void             ToggleLock() { _isLock = _isLock == true ? false : true; }

    /*                      사이즈 조정 설정 */
    inline void             SetSize(const ImVec2& size) { _size = {true, size}; }
    inline ImVec2           GetSize() { return _size.second; }

    /*                      위치 조정 설정 */
    inline void             SetPos(const ImVec2& pos) { _pos = {true, pos}; }
    inline ImVec2           GetPos() { return _pos.second; }

    void SetDockSpace(EditorDockSpace* dockSpace) { _dockSpace = dockSpace; }

private:
    EditorDockSpace* _dockSpace = nullptr; // 도킹 스페이스 (부모 도킹스페이스)

    DockLayout              _dockLayout = DockLayout::NONE;         // 초기 Dock영역 (초기 도킹빌드시에만 사용하고 이후엔 사용 X)
    ImGuiWindowFlags        _windowFlags = ImGuiWindowFlags_None;   // ImGui윈도우 플래그 (ImGuiWindowFlags_NoCollapse는 항상 활성화)
    bool                    _isLock = false;                        // 해당 탭에 대한 입력을 막을지에 대한 여부
    std::pair<bool, ImVec2> _size   = {false, ImVec2(0, 0)};    // 사이즈 조정 여부와 사이즈
    std::pair<bool, ImVec2> _pos    = {false, ImVec2(0, 0)};    // 위치 조정 여부와 위치
private:
    void DefaultPopupFrame();
    void DefaultDebugFrame();

};

