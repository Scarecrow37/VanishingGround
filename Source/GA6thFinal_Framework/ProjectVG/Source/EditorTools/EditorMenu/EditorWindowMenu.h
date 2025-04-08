#pragma once
#include "UmFramework.h"

/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorMenuTools : public EditorMenuNode
{
public:
    EditorMenuTools()
    {
        SetCallOrder(0);
        SetMenuPath("Window");
        SetLabel("Tools");
    }
    virtual ~EditorMenuTools() = default;

public:
    virtual void OnMenu() override;
};