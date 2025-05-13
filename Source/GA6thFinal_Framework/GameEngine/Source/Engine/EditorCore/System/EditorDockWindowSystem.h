#pragma once
 
class EditorDockWindow;

class EditorDockWindowSystem : public IEditorCycle
{
    using DockTable = std::unordered_map<std::string, EditorDockWindow*>;
    using DockList  = std::vector<EditorTool*>;

public:
    EditorDockWindowSystem();
    ~EditorDockWindowSystem();

public:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnDrawGui() override;
    virtual void OnEndGui() override;

public:
    EditorDockWindow* RegisterDockWindow(const std::string& label, EditorDockWindow* parent = nullptr);
    EditorDockWindow* GetDockWindow(const std::string& label);
    EditorDockWindow* operator[](const std::string& label);

    const DockList&   GetDockWindowList() const;

private:
    DockTable         _dockWindowTable;
    DockList          _dockWindowList;
};