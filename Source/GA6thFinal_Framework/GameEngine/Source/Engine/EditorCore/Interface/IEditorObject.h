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
    virtual void OnInspectorViewEnter() {};

    virtual void OnInspectorStay() {};

    virtual void OnInspectorExit() {};
};