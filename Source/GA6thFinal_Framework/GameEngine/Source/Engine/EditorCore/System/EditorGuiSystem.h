#pragma once
 
class EditorDockWindow;

class EditorGuiSystem : public IEditorCycle
{
    using DockTable = std::unordered_map<std::string, EditorDockWindow*>;
    using DockList  = std::vector<EditorTool*>;

public:
    EditorGuiSystem();
    ~EditorGuiSystem();

public:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnDrawGui() override;
    virtual void OnEndGui() override;

public:
    EditorDockWindow* RegisterDockWindow(const std::string& label, EditorDockWindow* parent = nullptr);
    EditorDockWindow* GetDockWindow(const std::string& label) const;
    EditorDockWindow* operator[](const std::string& label) const;

    YAML::Node SaveGuiSettingToMemory();
    void       LoadGuiSettingFromMemory(YAML::Node node);

    const DockList&   GetDockWindowList() const;
    const DockTable&  GetDockWindowTable() const;

private:
    DockTable         _dockWindowTable;
    DockList          _dockWindowList;

    std::string       _guiSettingDataFromString;

};