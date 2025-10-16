#pragma once

class EditorGuiSystem : public IEditorCycle
{
    using DockTable = std::unordered_map<DockID, EditorDockWindow*>;
    using DockList  = std::vector<EditorDockWindow*>;

public:
    EditorGuiSystem();
    ~EditorGuiSystem();

public:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnDrawGui() override;
    virtual void OnEndGui() override;

public:
    EditorDockWindow*   CreateDockWindow(const char* label, const char* parentLabel = nullptr);
    EditorDockWindow*   GetDockWindow(const char* label) const;
    EditorDockWindow*   operator[](const char* label) const;

    void                Clear();
    void                ResetLayout();
    YAML::Node          SaveGuiSettingToMemory();
    void                LoadGuiSettingFromMemory(YAML::Node node);
    const DockList&     GetDockWindowList() const;
    const DockTable&    GetDockWindowTable() const;

private:
    DockList    _dockWindowList;    // 원본 도킹 윈도우 리스트
    DockTable   _dockWindowTable;   // 도킹 윈도우 ID 테이블

    std::string _guiSettingDataFromString;

};