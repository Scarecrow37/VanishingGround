#pragma once

class EditorInspectorTool
    : public EditorTool
{
public:
    EditorInspectorTool();
    virtual ~EditorInspectorTool();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrameBegin() override;

    virtual void OnPostFrameBegin() override;

    virtual void OnFrameEnd() override;

    virtual void OnFrameFocused() override;

public:
    static bool IsLockFocus() { return _isLockFocus; }

    static bool IsFocused(std::weak_ptr<IEditorObject> obj);

public:
    // Command
    static bool SetFocusObject(std::weak_ptr<IEditorObject> obj);

    static bool SetLockFocus(bool isLock);

private:
    static void ShowMenuBarFrame();

private:
    inline static bool _isLockFocus = false;

    inline static std::weak_ptr<IEditorObject> _focusedObject;
};

namespace Command
{
    namespace Inspector
    {
        class FocusObject : public UmCommand
        {
        public:
            FocusObject(std::weak_ptr<GameObject> oldWp, std::weak_ptr<GameObject> newWp)
                : UmCommand("Inspector Focused"), _oldFocused(oldWp), _newFocused(newWp)
            {
            }
            virtual ~FocusObject() override = default;

        public:
            // UmCommand을(를) 통해 상속됨
            virtual void Execute() override;
            virtual void Undo() override;

        protected:
            std::weak_ptr<GameObject> _oldFocused;
            std::weak_ptr<GameObject> _newFocused;
        };

        // 사용 X
        class LockFocus : public UmCommand
        {
        public:
            LockFocus(bool isLock) 
                : UmCommand("Inspector Lock Focus"), _isLock(isLock)
            {
            }
            ~LockFocus() override = default;

        public:
            // UmCommand을(를) 통해 상속됨
            void Execute() override;
            void Undo() override;

        private:
            bool _isLock;
        };
    }
}
