#pragma once
#include "UmFramework.h"

class EditorDebugView
    : public EditorTool
{
public:
    EditorDebugView();
    virtual ~EditorDebugView();
private:
    virtual void OnTickGui() override;

    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

    virtual void OnPopup() override;
};

