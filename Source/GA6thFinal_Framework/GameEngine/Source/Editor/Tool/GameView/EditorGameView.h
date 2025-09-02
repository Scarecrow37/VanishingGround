#pragma once

class CameraComponent;

class EditorGameView : public EditorTool
{
    enum CameraMode
    {
        CAMERA_MODE_DEFAULT,
        CAMERA_MODE_FREE_MANIPULATE,
    };
public:
    EditorGameView();
    virtual ~EditorGameView() override;

protected:
    void OnFrameRender() override;
    void OnFrameFocusStay() override;

private:
    CameraMode _cameraMode;
    EditorDynamicCamera* _freeCamera;
};