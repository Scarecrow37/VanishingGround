#pragma once

using EditorDockWindowFlags = int; // EditorDockWindow::Flags

class EditorDockWindow : public EditorTool
{
    using Super             = EditorTool;
    using GuiTable          = std::unordered_map<std::string, EditorGui*>;
    using GuiList           = std::vector<EditorGui*>;
    using ToolTable         = std::unordered_map<std::string, EditorTool*>;

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
        ImGuiDir    Direction = ImGuiDir::ImGuiDir_None;
        float       Ratio = 0.0f;
    };

public:
    EditorDockWindow();
    virtual ~EditorDockWindow();

public:
    virtual bool SerializeFromData(EditorToolSerializeData* data) override;
    virtual bool DeSerializeFromData(EditorToolSerializeData* data) override;

public:
    // EditorGui을(를) 통해 상속됨
    void OnTickGui() override final;
    void OnStartGui() override final;
    void OnEndGui() override final;

private:
    virtual void OnPreFrameBegin();
    virtual void OnPostFrameBegin();
    virtual void OnFrameRender();
    virtual void OnFrameEnd();

public:
    /* 툴을 등록합니다. */
    template <typename T, typename... Args > 
    T* RegisterGui(Args... args);

    /* 툴을 가져옵니다. */
    template <typename T> 
    T* GetGui();

    bool RegisterChildDockWindow(EditorDockWindow* childDockWindow);

    /* DockBuild */
    void CreateDockLayoutNode(ImGuiDir direction, float ratio);
    bool SetDockBuildWindow(const std::string& label, ImGuiDir direction);

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

private:
    GuiTable                            _editorGuiClassTable;       /* 등록된 툴 테이블 (클래스 이름) */
    GuiList                             _editorGuiList;             /* 등록된 툴 리스트 */
    ToolTable                           _editorToolTable;           /* 등록된 툴 리스트 (툴 이름) */

    bool                                _isDockBuilding = false;    /* 도킹 빌드 중인지 여부 */
    int                                 _dockWindowOptionFlags;     /* 도킹 윈도우 플래그 값 */

    ImGuiID                             _dockSplitMainID;           /* 메인 도킹영역에 대한 ID값 */
    ImGuiDockNodeFlags                  _userImGuiDockFlags;        /* 사용자 DockSpace 플래그 값 */
    ImGuiDockNodeFlags                  _imGuiDockFlags;            /* 최종 DockSpace 플래그 값 */

    std::vector<DockSplitInfo>          _dockSplitLayoutID;         /* 도킹 영역에 대한 ID값 */
    std::unordered_map<int, ImGuiID>    _dockSplitIDTable;          /* 도킹 영역에 대한 ID값 */

    

public:
    /* 옵션 플래그에 대한 설정 */
    inline void         SetDockWindowFlags(EditorDockWindowFlags flags) { _dockWindowOptionFlags = flags; }
    inline void         AddDockWindowFlags(EditorDockWindowFlags flags) { _dockWindowOptionFlags |= flags; }
    inline void         RemoveDockWindowOptionFlags(EditorDockWindowFlags flags) { _dockWindowOptionFlags &= ~flags; }
    inline bool         HasDockWindowFlags(EditorDockWindowFlags flags) { return (_dockWindowOptionFlags & flags) == flags; }
    inline const auto&  GetDockWindowOptionFlags() { return _dockWindowOptionFlags; }

    /* Dock에 대한 플래그 설정 */
    inline void         SetImGuiDockNodeFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags = flags; }
    inline void         AddImGuiDockNodeFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags |= flags; }
    inline void         RemoveImGuiDockNodeFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags &= ~flags; }
    inline bool         HasImGuiDockNodeFlag(ImGuiDockNodeFlags flags) { return (_userImGuiDockFlags & flags) == flags; }
    inline int          GetImGuiDockNodeFlag() { return _userImGuiDockFlags; }

    inline const auto&  GetRefGuiList() { return _editorGuiList; }
    inline const auto&  GetRefGuiTable() { return _editorGuiClassTable; }
    inline const auto&  GetRefToolList() { return _editorToolTable; }
};

template <typename T>
concept IsEditorGui = std::is_base_of_v<EditorGui, T>;
template <typename T>
concept IsEditorTool = IsEditorGui<T> && std::is_base_of_v<EditorTool, T>;

 template <typename T, typename... Args>
inline T* EditorDockWindow::RegisterGui(Args... args)
{
    static_assert(std::is_base_of_v<EditorGui, T>, "T is not a EditorGui.");

    T*          instance  = new T(args...);
    const char* typeName  = typeid(T).name();
    auto        itr       = _editorGuiClassTable.find(typeName);
    if (itr == _editorGuiClassTable.end())
    {
        _editorGuiClassTable[typeName]  = instance;
        _editorGuiList.push_back(instance);
        if constexpr (IsEditorTool<T>)
        {
            _editorToolTable[typeName] = instance;
            instance->SetOwnerDockWindow(this);
        }
    }
    else
    {
        delete instance;
        instance = dynamic_cast<T*>(itr->second);
    }
    return instance;
}

template <typename T>
inline T* EditorDockWindow::GetGui()
{
    static_assert(std::is_base_of_v<EditorGui, T>, "T is not a EditorGui.");
    auto itr = _editorGuiClassTable.find(typeid(T).name());
    if (itr == _editorGuiClassTable.end())
        return nullptr;
    return dynamic_cast<T*>(itr->second);
}