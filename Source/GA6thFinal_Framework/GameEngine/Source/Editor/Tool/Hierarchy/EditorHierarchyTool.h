#pragma once

class EditorHierarchyTool
    : public EditorTool
{
public:
    inline static std::weak_ptr<GameObject> HierarchyFocusObjWeak;
    EditorHierarchyTool();
    virtual ~EditorHierarchyTool();
    static void ImGuiNewGameObjectMenuItems();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrameBegin() override;

    virtual void OnPostFrameBegin() override;

    virtual void OnFrameEnd() override;

    virtual void OnFramePopupOpened() override;

    //드래그 드롭 이벤트
    void HierarchyDropEvent();

    //빈 공간 우클릭시
    void HierarchyRightClickEvent() const;

    ImGuiWindow* window = nullptr;
};

