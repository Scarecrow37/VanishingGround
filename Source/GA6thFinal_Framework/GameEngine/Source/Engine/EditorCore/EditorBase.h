#pragma once

/*
외부에서 에디터 툴에 나타내기 위한 인터페이스 객체
GameObject, Asset 등은 이를 상속해 InspectorView 등에 그려질 내용을 구현한다.
*/ 
class IEditorObject
{
public:
    IEditorObject() = default;
    virtual ~IEditorObject() = default;
public:
    /* InspectorView에 SetFocus 될 때 호출 */
    virtual void OnFocusInspectorView() {};
    /* InspectorView의 Draw단계에 호출 */
    virtual void OnDrawInspectorView() {};
};

/*
에디터에 출력되는 창(프레임) 단위의 객체에 상속하기 위한 객체
*/
class EditorBase
{
public:
    EditorBase() = default;
    virtual ~EditorBase() = default;
public:
    /* 항상 계층에 상관없이 매 틱마다 호출 */
    virtual void    OnTickGui() = 0;

    /* ImGui시스템이 초기화 된 후 호출 */
    virtual void    OnStartGui() = 0;

    /* Gui가 그려져야 할 때 호출 */
    virtual void    OnDrawGui() = 0;

    /* ImGui시스템이 닫히기 직전에 호출 */
    virtual void    OnEndGui() = 0;
public:
    /* 이름 설정 (기본적으로 중복을 비허용.) */
    inline void         SetLabel(const std::string& label) { _label = label; }
    inline const auto&  GetLabel() { return _label; }
    /* 활성화 여부 설정 */
    inline void         SetVisible(bool v) { _isVisible = v; }
    inline bool         GetVisible() { return _isVisible; }
    inline void         ToggleVisible() { _isVisible = _isVisible == true ? false : true; }
    /* 호출 순서 설정 */
    inline void         SetCallOrder(int i) { _callOrder = i; }
    inline int          GetCallOrder() { return _callOrder; }
protected:
    std::string         _label = "";            // 에디터 툴 이름 (기본적으로 전역 단위의 이름 중복을 허용하지 않음. 나중엔 uuid등으로 관리할지 고민 중)
    bool                _isVisible = true;      // 에디터 창의 렌더 여부
    int                 _callOrder = 0;         // OnDrawGui 호출 순서
};

