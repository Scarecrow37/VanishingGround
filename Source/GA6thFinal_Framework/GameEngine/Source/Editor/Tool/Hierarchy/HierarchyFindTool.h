#pragma once
class EditorDockWindow;

class HierarchyFindTool : public EditorTool
{
public:
    HierarchyFindTool();
    virtual ~HierarchyFindTool() override;

private:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnEndGui() override;

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

private:
    enum class FindType
    {
        NAME,
        TAG
    };
    void DrawFinder();
    void DrawFindList();
private:
    EditorDockWindow* _ownerDockWindow = nullptr;
    std::vector<std::weak_ptr<GameObject>> _findList;
};