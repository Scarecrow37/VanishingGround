#pragma once

class CameraComponent;

class EditorGameView : public EditorTool
{
public:
    enum CameraMode
    {
        CAMERA_MODE_DEFAULT,
        CAMERA_MODE_FREE_MANIPULATE,
    };
public:
    EditorGameView();
    virtual ~EditorGameView();

public:
    void SetCameraMode(CameraMode mode);

    void ShowPopupCameraMode();

protected:
    void OnFramePopupOpened() override;
    void OnFrameRender() override;
    void OnFrameFocusStay() override;

private:
    CameraMode _cameraMode;
    EditorDynamicCamera* _freeCamera;
};