#pragma once
#include "UmFramework.h"

class EditorSceneTool
    : public EditorTool
{
public:
    EditorSceneTool();
    virtual ~EditorSceneTool();
private:
    virtual void  OnStartGui() override;

    virtual void  OnPreFrame() override;

    virtual void  OnFrame() override;

    virtual void  OnPostFrame() override;

    virtual void OnFocus() override;
};

