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
    /// <summary>
    /// 조건에 상관 없이 매 틱마다 호출됩니다.
    /// </summary>
    virtual void OnTickGui() = 0;

    /// <summary>
    /// ImGui가 초기화된 직후에 호출됩니다.
    /// </summary>
    virtual void OnStartGui() = 0;

    /// <summary>
    /// Gui가 렌더링 될 때 호출됩니다.
    /// </summary>
    virtual void OnDrawGui() = 0;

    /// <summary>
    /// ImGui가 종료되기 직전에 호출됩니다.
    /// </summary>
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
    inline void ToggleVisible() { ReflectFields->_isVisible = ReflectFields->_isVisible == true ? false : true; }
    inline bool IsVisible() const { return ReflectFields->_isVisible; }

    /*          플래그 설정 */
    inline void SetEditorGuiFlags(UINT flags) { _editorGuiOptionFlags = flags; }
    inline void AddEditorGuiFlags(UINT flags) { _editorGuiOptionFlags |= flags; }
    inline void RemoveEditorGuiFlags(UINT flags) { _editorGuiOptionFlags &= ~flags; }
    inline int  GetEditorGuiFlags() const { return _editorGuiOptionFlags; }
    inline bool HasEditorGuiFlags(UINT flags) const { return _editorGuiOptionFlags & flags; }
};

