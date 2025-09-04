#pragma once

class EditorSceneTool;

class EditorSceneMenu : public EditorMenu
{
public:
    EditorSceneMenu()          = default;
    virtual ~EditorSceneMenu() = default;

public:
    virtual void OnStartGui() override;
    virtual void OnMenu() override;

private:
    EditorSceneTool* _sceneTool = nullptr;
    bool _isSceneCameraPopUp = false;
    bool _isSceneSkyBoxEditPopup = false;
    void SceneCameraPopUp();
    void SceneSkyBoxEditPopup();
};