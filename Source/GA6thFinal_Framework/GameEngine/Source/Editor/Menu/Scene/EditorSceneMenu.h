#pragma once

class EditorSceneMenuScenes : public EditorMenu
{
public:
    EditorSceneMenuScenes()
    {
        SetCallOrder(0);
        SetPath("Scene");
        SetLabel("Scene");
    }
    virtual ~EditorSceneMenuScenes() = default;

public:
    virtual void OnMenu() override;
};

/*
 게임 오브젝트 생성 메뉴
*/
class EditorSceneMenuGameObject : public EditorMenu
{
public:
    static void ImGuiNewGameObjectMenuItems();

public:
    EditorSceneMenuGameObject()
    {
        SetCallOrder(1);
        SetPath("Scene");
        SetLabel("Scene");
    }
    virtual ~EditorSceneMenuGameObject() = default;

public:
    virtual void OnMenu() override;
};
