#pragma once
#include "UmFramework.h"

class EditorInspectorView
    : public EditorTool
{
public:
    EditorInspectorView();
    virtual ~EditorInspectorView();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

    virtual void OnFocus() override;

    void         TestGui();

public:
    static void SetFocusObject(std::weak_ptr<IEditorObject> obj)
    {
        _focusedObject = obj;
    }

private:
    inline static std::weak_ptr<IEditorObject> _focusedObject;
};

