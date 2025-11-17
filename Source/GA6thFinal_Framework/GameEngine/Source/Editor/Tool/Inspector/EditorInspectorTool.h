#pragma once

class EditorInspectorTool
    : public EditorTool
{
public:
    EditorInspectorTool();
    virtual ~EditorInspectorTool();
private:
    void OnEndGui() override;

    void OnPostFrameBegin() override;

    void OnFrameRender() override;

    void OnFrameEnd() override;

public:
    /// <summary>
    /// 포커스가 잠겨 있는지 여부를 반환합니다.
    /// </summary>
    /// <returns>포커스가 잠겨 있으면 true, 그렇지 않으면 false를 반환합니다.</returns>
    static bool IsLockFocus() { return _isLockFocus; }

    /// <summary>
    /// 포커스 잠금 상태를 설정합니다.
    /// </summary>
    /// <param name="isLock">포커스를 잠글지 여부를 지정하는 불리언 값입니다. true이면 포커스를 잠그고, false이면 잠금을 해제합니다.</param>
    /// <returns>포커스 잠금 설정이 성공하면 true, 실패하면 false를 반환합니다.</returns>
    static bool SetLockFocus(bool isLock);

    /// <summary>
    /// 지정된 객체가 현재 포커스된 객체인지 확인합니다.
    /// </summary>
    /// <param name="obj">포커스 여부를 확인할 IEditorObject의 weak_ptr입니다.</param>
    /// <returns>객체가 포커스된 경우 true, 그렇지 않으면 false를 반환합니다.</returns>
    static bool IsFocusObject(std::weak_ptr<IEditorObject> obj);

    /// <summary>
    /// 포커스 객체를 설정합니다.
    /// </summary>
    /// <param name="obj">포커스로 설정할 IEditorObject의 weak_ptr입니다.</param>
    /// <param name="breakLock">true로 설정하면 잠금을 해제하고 포커스를 변경합니다. 기본값은 false입니다.</param>
    /// <returns>포커스 설정에 성공하면 true, 실패하면 false를 반환합니다.</returns>
    static bool SetFocusObject(std::weak_ptr<IEditorObject> obj, bool breakLock = false);

    /// <summary>
    /// 포커스 객체를 초기화합니다. 잠금되어 있는 경우 무시됩니다.
    /// </summary>
    static void ResetFocusObject();

    /// <summary>
    /// 포커스된 객체를 나타내는 약한 포인터를 반환합니다.
    /// </summary>
    /// <returns>포커스된 IEditorObject를 가리키는 std::weak_ptr입니다. 포커스된 객체가 없으면 비어 있을 수 있습니다.</returns>
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
            FocusObject(std::weak_ptr<GameObject> oldWp, std::weak_ptr<GameObject> newWp, std::string_view commandName = "Inspector Focused")
                : UmCommand(commandName), _oldFocused(oldWp), _newFocused(newWp)
            {
            }
            virtual ~FocusObject() override = default;

        public:
            // UmCommand을(를) 통해 상속됨
            virtual bool Execute() override;
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
            bool Execute() override;
            void Undo() override;

        private:
            bool _isLock;
        };
    }
}
