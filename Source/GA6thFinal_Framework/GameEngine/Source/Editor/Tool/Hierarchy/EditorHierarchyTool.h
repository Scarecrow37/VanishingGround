#pragma once
class EditorSceneTool;
class HierarchyFindTool;

class EditorHierarchyTool
    : public EditorTool
{
public:
    inline static std::weak_ptr<GameObject> HierarchyFocusObjWeak;
    EditorHierarchyTool();
    virtual ~EditorHierarchyTool();

public:
    static void ImGuiNewGameObjectMenuItems();

    /*포커싱된 오브젝트의 트리 노드를 1회 Open 합니다.*/
    void OpenFocusObjectTree() { _isOpenFocusObj = true; }

private:
    void TransformTreeNode(Transform& node, const std::shared_ptr<GameObject>& focusObject);

    virtual void OnStartGui() override;

    virtual void OnPreFrameBegin() override;

    virtual void OnPostFrameBegin() override;

    virtual void OnFrameRender() override;

    virtual void OnFrameEnd() override;

    virtual void OnFramePopupOpened() override;

    virtual void OnTickGui() override;

    virtual void OnFrameFocusStay() override;

    //드래그 드롭 이벤트
    void HierarchyDropEvent();

    //빈 공간 우클릭시
    void HierarchyRightClickEvent() const;

    //키보드 이벤트
    void KeyboardEvent();

    ImGuiWindow* _window = nullptr;
    bool         _isPlay = false;
    bool         _isOpenFocusObj = false;

    EditorDockWindow* _dockWindow = nullptr;
    EditorSceneTool*  _editorSceneTool = nullptr;
    HierarchyFindTool* _editorFindTool = nullptr;
};

