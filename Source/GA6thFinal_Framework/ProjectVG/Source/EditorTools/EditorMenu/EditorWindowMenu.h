#pragma once
#include "UmFramework.h"

/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuTools : public EditorMenu
{
public:
    EditorMenuTools()
    {
        SetCallOrder(0);
        SetLabel("Window");
    }
    virtual ~EditorMenuTools() = default;

public:
    virtual void OnMenu() override;
};