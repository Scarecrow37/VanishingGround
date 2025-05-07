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