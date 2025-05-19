#pragma once

class GameObject;
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

    virtual void OnFrameRender() override;

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

    //clientSize
    float _clientWidth   = 0.f;    
    float _clientHeight  = 0.f;
    float _clientLeft    = 0.f;
    float _clientRight   = 0.f;
    float _clientTop     = 0.f;
    float _clientBottom  = 0.f;

    // Manipulate
    std::weak_ptr<GameObject> _manipulateObject;
    ImGuiHelper::DrawManipulateDesc drawManipulateDesc; 
    bool _isUseManipulate = false;
    bool _isUsingStart = false; 
    bool _isUsingEnd = false; 
    bool _isUsing = false;
    bool _isOver = false;

public:
    class ManipulateCommand : public UmCommand
    {
    public:
        struct Transform
        {
            Vector3 Position;
            Quaternion Rotation;
            Vector3 Scale;
        };
        ManipulateCommand(const std::shared_ptr<GameObject>& target, ManipulateCommand::Transform& curr, ManipulateCommand::Transform& prev);
        virtual ~ManipulateCommand();

    private:
        std::weak_ptr<GameObject>    _target;
        ManipulateCommand::Transform _prev;
        ManipulateCommand::Transform _curr;

        void Execute() override;
        void Undo() override;
    };
    
protected:
    REFLECT_FIELDS_BEGIN(EditorTool)
    std::array<float, 3> CameraPosition{0, 0, 0};
    std::array<float, 4> CameraRotation{0, 0, 0, 1};
    float  CameraFov   = 70.f;
    float  CameraAspect      = 1.0f;
    float  CameraNearZ       = 0.01f;
    float  CameraFarZ        = 10000.f;
    float  CameraMoveSpeed   = 30.0f;
    float  CameraRotateSpeed = 10.0f;
    REFLECT_FIELDS_END(EditorSceneTool)

    /*
    직렬화 직전 자동으로 호출되는 이벤트 함수입니다.
    직접 override 해서 사용합니다.
    */
    virtual void SerializedReflectEvent();
    /*
    역직렬화 이후 자동으로 호출되는 이벤트 함수 입니다.
    직접 override 해서 사용합니다.
    */
    virtual void DeserializedReflectEvent();

public:
    REFLECT_PROPERTY(
        ReflectFields->CameraFov, 
        ReflectFields->CameraNearZ,
        ReflectFields->CameraFarZ, 
        ReflectFields->CameraRotateSpeed
        )

    void UpdateCameraSetting();
};

