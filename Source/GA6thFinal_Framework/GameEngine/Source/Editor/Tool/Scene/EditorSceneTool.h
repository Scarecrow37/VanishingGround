#pragma once

class EditorSceneTool
    : public EditorTool
{
public:
    EditorSceneTool();
    virtual ~EditorSceneTool();
private:
    virtual void  OnStartGui() override;

    virtual void  OnPreFrame() override;

    virtual void  OnFrame() override;

    virtual void  OnPostFrame() override;

    virtual void OnFocus() override;

private:
    void ProcessInput();

    void ProcessViewManipulate();

    void ProcessManipulate();

private:
    Camera* _camera = nullptr;
    Matrix  _view;
    float   _cameraSpeed;

    // Camera
    float _fovDegree = 27.0f;
    float _aspect = 1.0f;
    float _nearZ = 0.1f;
    float _farZ = 100.f;

    // ViewManipulate
    float _setaDistance = 100.0f;

    // Manipulate
    bool                _useSnap;   
    float               _snap[3] = {1.f, 1.f, 1.f};
    ImGuizmo::MODE      _manipulateMode;
    ImGuizmo::OPERATION _manipulateOperation;

    Matrix _temMatrix;
};

