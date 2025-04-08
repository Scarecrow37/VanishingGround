#pragma once
#include "UmFramework.h"

class EditorHierarchyView
    : public EditorTool
{
public:
    EditorHierarchyView();
    virtual ~EditorHierarchyView();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

    virtual void OnPopup() override;
};

