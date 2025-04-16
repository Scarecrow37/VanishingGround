#pragma once

class EditorHierarchyTool
    : public EditorTool
{
public:
    EditorHierarchyTool();
    virtual ~EditorHierarchyTool();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

    virtual void OnPopup() override;

    void HierarchyDropEvent();
};

