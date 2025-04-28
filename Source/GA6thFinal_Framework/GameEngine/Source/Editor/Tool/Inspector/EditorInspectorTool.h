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
    static bool IsLockFocus() { return _isLockFocus; }

    static bool SetFocusObject(std::weak_ptr<IEditorObject> obj);

private:
    static void ShowMenuBarFrame();

private:
    inline static bool _isLockFocus = false;

    inline static std::weak_ptr<IEditorObject> _focusedObject;
};

