#pragma once

class EditorModelInspector : public EditorTool
{
public:
    EditorModelInspector();
    virtual ~EditorModelInspector() = default;

public:
    /* Begin 호출 전에 호출 */
    void OnPreFrameBegin() override;

    /* Begin 호출 직후 호출 */
    void OnPostFrameBegin() override;

    /* Begin 호출 후 클리핑 테스트를 통과한 후 호출 */
    void OnFrameRender() override;
    void OnFrameClipped() override;

    /* End 호출 후에 호출 */
    void OnFrameEnd() override;

    /* 프레임이 포커싱 될 때 호출 (OnPostFrameBegin 후에 호출) */
    void OnFrameFocusEnter() override;
    void OnFrameFocusStay() override;
    void OnFrameFocusExit() override;

    /* Popup창 호출 성공 시 호출 (OnPreFrameBegin 전에 호출) */
    void OnFramePopupOpened() override;

public:
    void SetSelectedObject(std::weak_ptr<IEditorObject> object);
    inline std::weak_ptr<IEditorObject> GetSelectedObject() const { return _focusedObject; }

private:
    std::weak_ptr<IEditorObject> _focusedObject; // 현재 틱에 포커스된 오브젝트
    IEditorObject*               _rowPtrFocused = nullptr; // 다음 틱에 포커스될 오브젝트의 Row Pointer

};