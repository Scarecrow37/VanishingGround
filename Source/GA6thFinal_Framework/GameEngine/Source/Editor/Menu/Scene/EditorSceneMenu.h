#pragma once

class EditorSceneTool;

class EditorSceneMenuScenes : public EditorMenu
{
public:
    EditorSceneMenuScenes()          = default;
    virtual ~EditorSceneMenuScenes() = default;

public:
    virtual void OnMenu() override;

private:
    EditorSceneTool* _sceneTool = nullptr;
    bool _isSceneCameraPopUp = false;
    void SceneCameraPopUp();
};