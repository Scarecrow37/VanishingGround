#pragma once

/*
에디터에 출력되는 창(프레임) 단위의 객체에 상속하기 위한 객체
*/
class EditorGui 
    : public IEditorCycle
    , public ReflectSerializer
{
protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(EditorGui)

public:
    EditorGui() = default;
    virtual ~EditorGui() = default;

public:
    virtual bool SerializeFromData(EditorToolSerializeData* data);
    virtual bool DeSerializeFromData(EditorToolSerializeData* data);

public:
    virtual void OnTickGui() = 0;
    virtual void OnStartGui() = 0;
    virtual void OnDrawGui() = 0;
    virtual void OnEndGui() = 0;

public:
    /*          활성화 여부 설정 */
    inline void SetVisible(bool v) { _isVisible = v; }
    inline bool IsVisible() { return _isVisible; }
    inline void ToggleVisible() { _isVisible = _isVisible == true ? false : true; }

protected:
    bool _isVisible = true;      // 에디터 창의 렌더 여부
    
};

