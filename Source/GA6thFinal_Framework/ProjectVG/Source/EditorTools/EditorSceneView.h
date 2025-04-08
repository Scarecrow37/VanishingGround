#pragma once
#include "UmFramework.h"

class EditorSceneView
    : public EditorTool
{
public:
    EditorSceneView();
    virtual ~EditorSceneView();
private:
    virtual void  OnStartGui() override;

    virtual void  OnPreFrame() override;

    virtual void  OnFrame() override;

    virtual void  OnPostFrame() override;

    virtual void OnFocus() override;
};

