#pragma once

using EditorDockWindowFlags = int; // EditorDockWindow::Flags
using DockID = ImGuiID;

class EditorDockWindow : public EditorTool
{
    using Super             = EditorTool;
    using GuiList           = std::vector<EditorGui*>;
    using GuiTable          = std::unordered_map<std::string, EditorGui*>;
    using ToolTable         = std::unordered_map<std::string, EditorTool*>;
    using MenuTable         = std::unordered_map<std::string, EditorMenu*>;
    using DockWindowTable   = std::unordered_map<std::string, EditorDockWindow*>;

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
    // EditorGui을(를) 통해 상속됨
    void OnTickGui() override final;
    void OnStartGui() override final;
    void OnEndGui() override final;

private:
    virtual void OnPreFrameBegin() override;
    virtual void OnPostFrameBegin() override;
    virtual void OnFrameRender() override;
    virtual void OnFrameEnd() override;
    virtual void OnFrameFocusStay() override;

    virtual void ProcessFocusFrame() override;

public:
    /* 툴을 등록합니다. */
    template <typename T, typename... Args> requires std::is_base_of_v<EditorGui, T>
    bool RegisterGui(Args... args);

    /* 툴을 가져옵니다. */
    template <typename T> requires std::is_base_of_v<EditorGui, T>
    T* GetGui() const;

    bool RegisterChildDockWindow(EditorDockWindow* childDockWindow);

    /* DockBuild */
    bool IsBuildingDockLayout() const;
    void CreateDockLayoutNode(ImGuiDir direction, float ratio);
    bool SetGuiDockLayout(EditorTool* tool);
    ImGuiID GetDockSplitID(int split) const;
    void RequestBuildDockLayout();

private:
    /* 초기 툴의 DockSpace 공간 빌드 */
    bool BeginBuildDockLayout();
    void EndBuildDockLayout();
    /* DockSpace를 현재의 Flag, Style 기반으로 등록 */
    void SubmitDockSpace();
    /* DockWindow Flag 업데이트 (SubmitDockSpace 이전에 호출해야 함) */
    void UpdateFlag();
    /* DockSytle Push (SubmitDockSpace 이전에 호출해야 함) */
    void PushDockStyle();
    /* DockSytle Pop (SubmitDockSpace 이후에 호출해야 함) */
    void PopDockStyle();

private:
    GuiList                             _editorGuiList;             /* 등록된 원본 툴 리스트 */
    GuiTable                            _editorGuiClassTable;       /* 등록된 툴 테이블 (클래스 이름) */
    ToolTable                           _editorToolTable;           /* 등록된 툴 리스트 */
    MenuTable                           _editorMenuTable;           /* 등록된 메뉴 리스트 */
    DockWindowTable                     _dockWindowTable;           /* 등록된 도킹 윈도우 리스트 */

    bool                                _needBuildDockLayout;       /* 도킹 빌드 중인지 여부 */
    bool                                _isBuildingDockLayout;      /* 도킹 빌드 중인지 여부 */
    int                                 _dockWindowOptionFlags;     /* 도킹 윈도우 플래그 값 */

    ImGuiID                             _dockSplitMainID;           /* 메인 도킹영역에 대한 ID값 */
    ImGuiDockNodeFlags                  _userImGuiDockFlags;        /* 사용자 DockSpace 플래그 값 */
    ImGuiDockNodeFlags                  _imGuiDockFlags;            /* 최종 DockSpace 플래그 값 */

    std::vector<DockSplitInfo>          _dockSplitLayoutID;         /* 도킹 영역에 대한 ID값 */
    std::unordered_map<int, ImGuiID>    _dockSplitIDTable;          /* 도킹 영역에 대한 ID값 */

private:
    int _pushedStyleCount = 0;                                      // [internal] PushStyleVar 호출 횟수

public:
    /* 옵션 플래그에 대한 설정 */
    inline void                         SetDockWindowFlags(EditorDockWindowFlags flags) { _dockWindowOptionFlags = flags; }
    inline void                         AddDockWindowFlags(EditorDockWindowFlags flags) { _dockWindowOptionFlags |= flags; }
    inline void                         RemoveDockWindowOptionFlags(EditorDockWindowFlags flags) { _dockWindowOptionFlags &= ~flags; }
    inline bool                         HasDockWindowFlags(EditorDockWindowFlags flags) const { return (_dockWindowOptionFlags & flags) == flags; }
    inline EditorDockWindowFlags        GetDockWindowOptionFlags() const { return _dockWindowOptionFlags; }

    /* Dock에 대한 플래그 설정 */
    inline void                         SetImGuiDockNodeFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags = flags; }
    inline void                         AddImGuiDockNodeFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags |= flags; }
    inline void                         RemoveImGuiDockNodeFlag(ImGuiDockNodeFlags flags) { _userImGuiDockFlags &= ~flags; }
    inline bool                         HasImGuiDockNodeFlag(ImGuiDockNodeFlags flags) const { return (_userImGuiDockFlags & flags) == flags; }
    inline ImGuiDockNodeFlags           GetImGuiDockNodeFlag() const { return _userImGuiDockFlags; }

    inline const GuiList&               GetRefGuiList() const { return _editorGuiList; }
    inline const GuiTable&              GetRefGuiTable() const { return _editorGuiClassTable; }
    inline const ToolTable&             GetRefToolTable() const { return _editorToolTable; }
    inline const MenuTable&             GetRefMenuTable() const { return _editorMenuTable; }
    inline const DockWindowTable&       GetRefDockWindowTable() const { return _dockWindowTable; }
};

template <typename T>
concept IsEditorGui = std::is_base_of_v<EditorGui, T>;
template <typename T>
concept IsEditorTool = IsEditorGui<T> && std::is_base_of_v<EditorTool, T>;
template <typename T>
concept IsEditorMenu = IsEditorGui<T> && std::is_base_of_v<EditorMenu, T>;
template <typename T>
concept IsEditorDockWindow = IsEditorGui<T> && std::is_base_of_v<EditorDockWindow, T>;

template <typename T, typename... Args> requires std::is_base_of_v<EditorGui, T>
inline bool EditorDockWindow::RegisterGui(Args... args)
{
    const char* typeName = typeid(T).name();
    if (false == _editorGuiClassTable.contains(typeName))
    {
        T* instance = new T(args...);
        _editorGuiClassTable[typeName] = instance;
        _editorGuiList.push_back(instance);
        if constexpr (IsEditorTool<T>)
        {
            // 에디터 툴인경우 자신을 부모로 등록
            _editorToolTable[typeName] = instance;
            instance->SetOwnerDockWindow(this);
        }
        else if constexpr (IsEditorMenu<T>)
        {
            _editorMenuTable[typeName] = instance;
        }
        return true;
    }
    else
    {
        return false;
    }
}

template <typename T> requires std::is_base_of_v<EditorGui, T>
inline T* EditorDockWindow::GetGui() const
{
    const char* typeName = typeid(T).name();
    auto itr = _editorGuiClassTable.find(typeName);
    if (itr == _editorGuiClassTable.end())
        return nullptr;
    return dynamic_cast<T*>(itr->second);
}