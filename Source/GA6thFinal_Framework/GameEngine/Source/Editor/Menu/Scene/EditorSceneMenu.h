#pragma once

class EditorSceneMenuScenes : public EditorMenu
{
public:
    EditorSceneMenuScenes()          = default;
    virtual ~EditorSceneMenuScenes() = default;

public:
    virtual void OnMenu() override;
};