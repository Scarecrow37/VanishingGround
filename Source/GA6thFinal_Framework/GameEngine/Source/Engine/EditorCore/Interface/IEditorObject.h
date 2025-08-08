#pragma once

/*
외부에서 에디터 툴에 나타내기 위한 인터페이스 객체
GameObject, Asset 등은 이를 상속해 InspectorView 등에 그려질 내용을 구현한다.
*/
class IEditorObject
{
public:
    IEditorObject()          = default;
    virtual ~IEditorObject() = default;

public:
    /// <summary>
    /// 인스펙터 ImGui::Begin 이후 바로 호출됩니다.
    /// </summary>
    virtual void OnInspectorBegin() {};

    /// <summary>
    /// 인스펙터 창에 선택될때 한번 호출됩니다.
    /// </summary>
    virtual void OnInspectorEnter() {};

    /// <summary>
    /// 인스펙터 창에 선택 되어있으면 계속 호출됩니다.
    /// </summary>
    virtual void OnInspectorStay() {};

    /// <summary>
    /// 인스펙터 창에서 선택 해제될때 한번 호출됩니다.
    /// </summary>
    virtual void OnInspectorExit() {};
};