#pragma once
#include "UmFramework.h"

/*
 게임 오브젝트 생성 메뉴
*/
class EditorSceneMenuGameObject : public EditorMenuNode
{
public:
    EditorSceneMenuGameObject()
    {
        SetCallOrder(1);
        SetMenuPath("Scene");
        SetLabel("Game Object");
    }
    virtual ~EditorSceneMenuGameObject() = default;

public:
    virtual void OnMenu() override;
};

class EditorSceneMenuScenes : public EditorMenuNode
{
public:
    EditorSceneMenuScenes()
    {
        SetCallOrder(0);
        SetMenuPath("Scene");
        SetLabel("Scene");
    }
    virtual ~EditorSceneMenuScenes() = default;

public:
    virtual void OnMenu() override;
};