#pragma once

class EditorInspectorTool
    : public EditorTool
{
public:
    EditorInspectorTool();
    virtual ~EditorInspectorTool();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

    virtual void OnFocus() override;

public:
    static void SetFocusObject(std::weak_ptr<IEditorObject> obj)
    {
        _focusedObject = obj;
    }

private:
    inline static std::weak_ptr<IEditorObject> _focusedObject;
};

