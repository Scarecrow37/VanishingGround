#pragma once

/*
도킹이 가능한 에디터 윈도우를 생성할 수 있는 객체
*/
class EditorTool : public EditorBase
{
    friend class EditorModule;
public:
    EditorTool() = default;
    virtual ~EditorTool() = default;
public:
    virtual void OnTickGui() override {}
    virtual void OnStartGui() override {};
    virtual void OnDrawGui() override;
    virtual void OnEndGui() override {};
protected:
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
    inline void             SetInitialDockSpaceArea(DockSpaceArea area) { _dockSpaceArea = area; }
    inline DockSpaceArea    GetInitialDockSapceArea() { return _dockSpaceArea; }
    /*                      플래그 설정 */
    inline void             SetWindowFlag(ImGuiWindowFlags flag) { _windowFlags = flag; }
    inline ImGuiWindowFlags AddWindowFlag(ImGuiWindowFlags flag) { _windowFlags |= flag; }

    /*                      툴 잠금 설정 */
    inline void             SetLock(bool v) { _isLock = v; }
    inline bool             GetLock() { return _isLock; }
    inline void             ToggleLock() { _isLock = _isLock == true ? false : true; }
protected:
    DockSpaceArea _dockSpaceArea = DockSpaceArea::NONE;     // 초기 DockSpace영역 (초기 도킹빌드시에만 사용하고 이후엔 사용 X)
    ImGuiWindowFlags _windowFlags = ImGuiWindowFlags_None;  // ImGui윈도우 플래그 (ImGuiWindowFlags_NoCollapse는 항상 활성화)
    bool _isLock = false;                                   // 해당 탭에 대한 입력을 막을지에 대한 여부        
    // + 추가할 것: 오프셋 등
private:
    void DefaultPopupFrame();
    void DefaultDebugFrame();
};

