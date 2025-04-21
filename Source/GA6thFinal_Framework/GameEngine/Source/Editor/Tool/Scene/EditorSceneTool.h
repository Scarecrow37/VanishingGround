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
    void ProcessMove();
    
    void ProcessMode();

    void ProcessViewManipulate();

    void ProcessManipulate();

private:
    Camera* _camera = nullptr;

    // Camera
    Vector3 _position = Vector3::Zero;
    Vector3 _diretion = Vector3::Forward;
    Matrix  _rotation = Matrix::Identity;
    Matrix _view;
    float _fovDegree = 27.0f;
    float _aspect = 1.0f;
    float _nearZ = 0.1f;
    float _farZ = 100.f;
    float _moveSpeed = 10.0f;
    float _rotateSpeed = 5.0f;

    // ViewManipulate
    float _setDistance = 100.0f;

    // Manipulate
    bool                _useSnap;   
    float               _snap[3] = {1.f, 1.f, 1.f};
    ImGuizmo::MODE      _manipulateMode;
    ImGuizmo::OPERATION _manipulateOperation;
    
    // tempObject
    Matrix _tempMatrix;
};

