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

    virtual void OnFrameRender() override;

    virtual void OnFrameEnd() override;

public:
    static bool IsLockFocus() { return _isLockFocus; }
    static bool SetLockFocus(bool isLock);

    static bool IsFocusObject(std::weak_ptr<IEditorObject> obj);
    static bool SetFocusObject(std::weak_ptr<IEditorObject> obj);
    static std::weak_ptr<IEditorObject> GetFocusObject();

private:
    static void ShowMenuBarFrame();

private:
    inline static std::weak_ptr<IEditorObject> _currFocused;    // 현재 틱에 포커스된 오브젝트
    inline static std::weak_ptr<IEditorObject> _nextFocused;    // 다음 틱에 포커스될 오브젝트
    inline static IEditorObject* _rowPtrCurrFocused = nullptr;  // 현재 틱에 포커스된 오브젝트의 Row Pointer
    inline static IEditorObject* _rowPtrNextFocused = nullptr;  // 다음 틱에 포커스될 오브젝트의 Row Pointer

    inline static bool _isLockFocus    = false; // 포커스 잠금 여부
    inline static bool _isFocusChanged = false; // 포커스 변경 여부
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
