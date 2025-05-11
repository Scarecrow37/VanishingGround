#pragma once

/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuTools : public EditorMenu
{
public:
    EditorMenuTools()
    {
        SetPath("Window");
    }
    virtual ~EditorMenuTools() = default;

public:
    virtual void OnMenu() override;
};