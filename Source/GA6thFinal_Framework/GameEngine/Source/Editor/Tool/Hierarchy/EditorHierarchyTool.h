#pragma once
class EditorSceneTool;
class HierarchyFindTool;
struct Scene;

class EditorHierarchyTool
    : public EditorTool
{
    inline static std::weak_ptr<GameObject> static_hierarchyFocusObjWeak;
    inline static bool                      static_isOpenFocusObj = false;

public:
    static void SetFocusObject(const std::weak_ptr<GameObject>& object);
    static const std::weak_ptr<GameObject>& GetFocusObject() { return static_hierarchyFocusObjWeak; }

    static EditorHierarchyTool* GetTool()
    {
        if constexpr (IS_EDITOR)
        {
            EditorDockWindow* sceneDock = Global::editorModule->GetDockWindowSystem().GetDockWindow("Scene##dock");
            if (sceneDock)
            {
                EditorHierarchyTool* editorHierarchy = sceneDock->GetGui<EditorHierarchyTool>();
                if (editorHierarchy)
                {
                    return editorHierarchy;
                }
            }
        }
        return nullptr;
    }

    /// <summary>
    /// 하이러키 오브젝트들을 정리합니다.
    /// </summary>
    static void CleanupEditorObjects()
    {
        if constexpr (IS_EDITOR)
        {
            EditorHierarchyTool* editorHierarchy = GetTool();
            if (editorHierarchy)
            {
                editorHierarchy->CleanupHierarchyObjects();
            }         
        }
    }

    /// <summary>
    /// 씬을 현재 상태로 저장합니다.
    /// </summary>
    /// <param name="scene"></param>
    /// <returns></returns>
    static bool SaveScene(Scene& scene);

    EditorHierarchyTool();
    virtual ~EditorHierarchyTool();

public:
    /*오브젝트 생성하는 Imgui Menu Item들을 Draw 합니다.*/
    static void ImGuiNewGameObjectMenuItems();

    /*포커싱된 오브젝트의 트리 노드를 1회 Open 합니다.*/
    void OpenFocusObjectTree() 
    { 
        static_isOpenFocusObj = true; 
    }

    /// <summary>
    /// 하이러키 오브젝트들을 정리합니다.
    /// </summary>
    void CleanupHierarchyObjects()
    {
        _hierarchySceneIndex.clear();
        _hierarchyRootObjects.clear();
        _hierarchyDontDestroyOnLoadObjects.clear();
    }

private:
    void TransformTreeNode(Transform& node, const std::shared_ptr<GameObject>& focusObject, GameObject*& outClickNode, bool isOpenFocusObject);

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

    //하이러키 트리 그리기
    void HierarchyDrawTreeNode();

    //키보드 이벤트
    void KeyboardEvent();

    ImGuiWindow* _window = nullptr;
    bool         _isPlay = false;

    EditorDockWindow* _dockWindow = nullptr;
    EditorSceneTool*  _editorSceneTool = nullptr;
    HierarchyFindTool* _editorFindTool = nullptr;

    //오브젝트 항목
    std::unordered_map<std::string, size_t>                                       _hierarchySceneIndex;
    std::vector<std::pair<std::string, std::vector<std::shared_ptr<GameObject>>>> _hierarchyRootObjects;
    std::vector<std::shared_ptr<GameObject>>                                      _hierarchyDontDestroyOnLoadObjects;

protected: 
    REFLECT_FIELDS_BEGIN(EditorTool)
    REFLECT_FIELDS_END(EditorHierarchyTool)
        
    /*
    직렬화 직전 자동으로 호출되는 이벤트 함수입니다.
    직접 override 해서 사용합니다.
    */
    virtual void SerializedReflectEvent() override;
    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent() override;

};

