#pragma once

class EditorDynamicCamera;
class EditorSceneTool
    : public EditorTool
{
public:
    EditorSceneTool();
    virtual ~EditorSceneTool();
private:
    virtual void  OnStartGui() override;

    virtual void OnPreFrameBegin() override;

    virtual void OnPostFrameBegin() override;

    virtual void OnFrameEnd() override;

    virtual void OnFrameFocused() override;

private:
    void ProcessMove();
    
    void ProcessMode();

    void ProcessViewManipulate();

    void ProcessManipulate();

private:
    bool _isHorverdScene = false;

    std::unique_ptr<EditorDynamicCamera> _camera;

    // Camera
    Vector3 _position = Vector3::Zero;
    Vector3 _diretion = Vector3::Forward;
    Vector3 _rotation=Vector3::Zero;
    Matrix _view;
    float _fovDegree = 45.f;
    float _aspect = 1.0f;
    float _nearZ = 0.01f;
    float _farZ = 10000.f;
    float _moveSpeed = 30.0f;
    float _rotateSpeed = 10.0f;

    // ViewManipulate
    float _setDistance = 100.0f;

    // Manipulate
    bool                _useSnap;   
    float               _snap[3] = {1.f, 1.f, 1.f};
    ImGuizmo::MODE      _manipulateMode;
    ImGuizmo::OPERATION _manipulateOperation;
    
    // tempObject
    Matrix _tempMatrix;

protected:
    //REFLECT_FIELDS_BEGIN(EditorTool)
    //std::array<float, 3> position{};
    //std::array<float, 4> rotation{};
    //std::array<float, 3> eulerAngle{};
    //std::array<float, 3> scale{};
    //REFLECT_FIELDS_END(EditorSceneTool)
};

