#pragma once

class EditorDynamicCamera;
class EditorSceneTool
    : public EditorTool
{
public:
    EditorSceneTool();
    virtual ~EditorSceneTool();

    static void SetManipulateObject(std::weak_ptr<GameObject>& object);
private:
    inline static EditorSceneTool* pSceneTool = nullptr;

private:
    virtual void  OnStartGui() override;

    virtual void OnPreFrameBegin() override;

    virtual void OnPostFrameBegin() override;

    virtual void OnFrameEnd() override;

    virtual void OnFrameFocusStay() override;

private:
    void SetMoveFlag();
    void SetCamera();
    void UpdateMode();
    void DrawManipulate();
    void DrawSceneView();

private:
    bool _isHorverdScene = false;

    std::unique_ptr<EditorDynamicCamera> _camera;

    // Camera
    Vector3 _position = Vector3::Zero;
    Vector3 _diretion = Vector3::Forward;
    Vector3 _rotation=Vector3::Zero;

    float _fovDegree = 70.f;
    float _aspect = 1.0f;
    float _nearZ = 0.01f;
    float _farZ = 10000.f;
    float _moveSpeed = 30.0f;
    float _rotateSpeed = 10.0f;

    // Manipulate
    std::weak_ptr<GameObject> _manipulateObject;
    ImGuiHelper::DrawManipulateDesc drawManipulateDesc; 
    bool _isUsing = false;
    bool _isOver = false;
    
    //clientSize
    float _clientWidth   = 0.f;    
    float _clientHeight  = 0.f;
    float _clientLeft    = 0.f;
    float _clientRight   = 0.f;
    float _clientTop     = 0.f;
    float _clientBottom  = 0.f;

protected:
    //REFLECT_FIELDS_BEGIN(EditorTool)
    //std::array<float, 3> position{};
    //std::array<float, 4> rotation{};
    //std::array<float, 3> eulerAngle{};
    //std::array<float, 3> scale{};
    //REFLECT_FIELDS_END(EditorSceneTool)
};

