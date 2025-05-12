#pragma once

class EditorDockSpace : public EditorGui
{
    using ToolTable = std::unordered_map<std::string, std::unique_ptr<EditorTool>>;
public:
    EditorDockSpace();
    virtual ~EditorDockSpace();
public:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnDrawGui() override;
    virtual void OnEndGui() override;
public:
    /* 툴을 등록합니다. */
    template <typename T>
    T* RegisterTool()
    {
        static_assert(std::is_base_of_v<EditorTool, T>, "T is not a EditorTool.");
        const char* typeName = typeid(T).name();
        auto itr = _editorToolTable.find(typeName);
        if (itr == _editorToolTable.end())
        {
            T* instance = new T;
            _editorToolTable[typeName].reset(instance);
            _editorToolList.push_back(instance);
        }
        return GetTool<T>();
    }

    /* 툴을 가져옵니다. */
    template <typename T>
    T* GetTool()
    {
        static_assert(std::is_base_of_v<EditorTool, T>, "T is not a EditorGui.");
        auto itr = _editorToolTable.find(typeid(T).name());
        if (itr == _editorToolTable.end())
            return nullptr;
        return dynamic_cast<T*>(itr->second.get());
    }
    EditorTool* GetTool(const std::string& name);
   

    inline const auto& GetRefToolTable() { return _editorToolTable; }
private:
    ToolTable _editorToolTable;                     /* 검색용 툴 컨테이너 */
    std::vector<EditorGui*> _editorToolList;       /* 순회용 툴 리스트 */
    bool _isFullSpace;                              /* DockSpace가 화면 전체를 차지하는지 여부 */
    bool _isPadding;                                /* DockSpace가 Padding을 할지 여부 */
    ImGuiDockNodeFlags _dockNodeFlags;              /* DockSpace 플래그 값 */
    ImGuiWindowFlags _dockWindowFlags;              /* DockWindow 플래그 값 */
    ImGuiID _dockSpaceMainID;                       /* 메인 도킹영역 ID값 */
    ImGuiID _dockLayoutID[(INT)DockLayout::END];    /* 도킹 영역에 대한 ID값 */
    const char* _dockAreaInitalData;                /* (미구현) 초기 도킹 세팅 저장 값 */
private:
    /* 최초로 에디터를 킬 경우 초기 툴의 DockSpace 공간 지정 */
    void InitDockLayout();
    /* DockSpace를 현재의 Flag, Style 기반으로 등록 */
    void SubmitDockSpace();
    /* DockWindow Flag 업데이트 (SubmitDockSpace 이전에 호출해야 함) */
    void UpdateWindowFlag();
    /* DockSytle Push (SubmitDockSpace 이전에 호출해야 함) */
    void PushDockStyle();
    /* DockSytle Pop (SubmitDockSpace 이후에 호출해야 함) */
    void PopDockStyle();
public:
    /* DockSpace 초기 창 위치 복원 */
    inline void ResetDockBuild() { ImGui::LoadIniSettingsFromMemory(_dockAreaInitalData); }
    /* DockSpace가 윈도우 전체를 차지하도록 설정함. */
    inline void SetDockFullSpace(bool isFullSpace) { _isFullSpace = isFullSpace; }
    inline auto IsDockFullSpace() { return _isFullSpace; }
    /* DockSpace가 윈도우 전체를 차지하도록 설정함. */
    inline void SetDockPadding(bool isPadding) { _isPadding = isPadding; }
    inline auto IsDockPadding() { return _isPadding; }
    /* Dock에 대한 플래그 설정 */
    inline void SetDockFlag(ImGuiDockNodeFlags flags) { _dockNodeFlags = flags; }
    /* 메인 DockSpace의 ID 반환 */
    inline ImGuiID GetDockSpaceID() { return _dockSpaceMainID; }
    /* 메인 DockSpace의 특정 영역 ID 반환 */
    inline ImGuiID GetDockLayoutID(DockLayout area) { return _dockLayoutID[(INT)area]; }
};

