#pragma once

/*
에디터에 출력되는 창(프레임) 단위의 객체에 상속하기 위한 객체
*/
class EditorGui 
    : public IEditorCycle
    , public ReflectSerializer
{
public:
    enum FLags
    {
        EDITORGUI_FLAGS_NONE           = 0,
    };
public:
    EditorGui() = default;
    virtual ~EditorGui() = default;

public:
    virtual void OnTickGui() = 0;
    virtual void OnStartGui() = 0;
    virtual void OnDrawGui() = 0;
    virtual void OnEndGui() = 0;

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    bool _isVisible = true;
    REFLECT_FIELDS_END(EditorGui)

private:
    int _editorGuiOptionFlags = EDITORGUI_FLAGS_NONE; // 옵션 플래그

public:
    /*          활성화 여부 설정 */
    inline void SetVisible(bool v) {ReflectFields->_isVisible = v; }
    inline bool IsVisible() { return ReflectFields->_isVisible; }
    inline void ToggleVisible() { ReflectFields->_isVisible = ReflectFields->_isVisible == true ? false : true; }

    /*          플래그 설정 */
    inline void SetEditorGuiFlags(UINT flags) { _editorGuiOptionFlags = flags; }
    inline void AddEditorGuiFlags(UINT flags) { _editorGuiOptionFlags |= flags; }
    inline void RemoveEditorGuiFlags(UINT flags) { _editorGuiOptionFlags &= ~flags; }
    inline int  GetEditorGuiFlags() { return _editorGuiOptionFlags; }
    inline bool HasEditorGuiFlags(UINT flags) { return _editorGuiOptionFlags & flags; }
};

