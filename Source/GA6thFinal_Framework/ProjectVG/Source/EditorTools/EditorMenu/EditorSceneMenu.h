#pragma once
#include "UmFramework.h"

/*
에디터 툴을 끄고 키는 메뉴
*/
class EditorSceneMenu : public EditorMenu
{
public:
    EditorSceneMenu()
    {
        SetCallOrder(1);
        SetLabel("Scene/GameObject");
    }
    virtual ~EditorSceneMenu() = default;

public:
    virtual void OnMenu() override;
};