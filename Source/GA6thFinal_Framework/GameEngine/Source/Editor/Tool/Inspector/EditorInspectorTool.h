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

    static bool IsFocused(std::weak_ptr<IEditorObject> obj);

private:
    static void ShowMenuBarFrame();

private:
    inline static bool _isLockFocus = false;

    inline static std::weak_ptr<IEditorObject> _focusedObject;
};

namespace Command
{
    class FocusInspecor : public UmCommand
    {
    public:
        FocusInspecor(std::weak_ptr<GameObject> oldWp, std::weak_ptr<GameObject> newWp)
            : UmCommand("Inspector Focused"), _oldFocused(oldWp), _newFocused(newWp)
        {
        }
        ~FocusInspecor() override = default;

    public:
        // UmCommand을(를) 통해 상속됨
        void Execute() override;
        void Undo() override;

    private:
        std::weak_ptr<GameObject> _oldFocused;
        std::weak_ptr<GameObject> _newFocused;
    };
}
