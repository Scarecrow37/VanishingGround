#pragma once
#include "UmFramework.h"

/*
 게임 오브젝트 생성 메뉴
*/
class EditorSceneMenuGameObject : public EditorMenu
{
public:
    EditorSceneMenuGameObject()
    {
        SetCallOrder(1);
        SetLabel("Scene");
    }
    virtual ~EditorSceneMenuGameObject() = default;

public:
    virtual void OnMenu() override;
};

class EditorSceneMenuScenes : public EditorMenu
{
public:
    EditorSceneMenuScenes()
    {
        SetCallOrder(0);
        SetLabel("Scene");
    }
    virtual ~EditorSceneMenuScenes() = default;

public:
    virtual void OnMenu() override;
};
