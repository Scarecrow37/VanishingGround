#pragma once

class GameObject;
class EditorDynamicCamera;
class MeshComponent;
class BaseMesh;
class EditorHierarchyTool;

class EditorSceneTool
    : public EditorTool
{
    USING_PROPERTY(EditorSceneTool)
public:
    EditorSceneTool();
    virtual ~EditorSceneTool();

    static void SetManipulateObject(std::weak_ptr<GameObject> object);

public:
    const Matrix& GetCameraMatrix();
    void SetCameraToObject(std::weak_ptr<GameObject> destination);
    bool          IsMouseHoveredWindow() const { return _isHoveredWindow; }

private:
    inline static EditorSceneTool* pSceneTool = nullptr;

private:
    void OnTickGui() override;

    void OnStartGui() override;

    void OnPreFrameBegin() override;

    void OnPostFrameBegin() override;

    void OnFrameRender() override;

    void OnFrameEnd() override;

    void OnFrameFocusStay() override;

private:
    void LoadDefaultIcon();

    void UpdateKeyboardFrameFocus();
    void UpdateKeyboardFrameRender();
    void DragDropEvent();
    void SetMoveFlag();
    void SetCamera();
    void DrawManipulate();
    void DrawSceneView();
    void RayPicker();
    void VertexSnap();

    bool IsActiveOperation(ImGuizmo::OPERATION op) const;
    bool IsActiveMode(ImGuizmo::MODE mode) const;

private:
    ImGuiWindow* _window = nullptr;
    EditorDockWindow* _dockWindow = nullptr;
    EditorHierarchyTool* _editorHierarchyTool = nullptr;

    bool _isHoveredWindow = false;
    std::unique_ptr<EditorDynamicCamera> _camera;

    //clientSize
    float _sceneClientWidth  = 0.f;    
    float _sceneClientHeight = 0.f;
    float _sceneClienttLeft  = 0.f;
    float _sceneClientRight  = 0.f;
    float _sceneClientTop    = 0.f;
    float _sceneClientBottom = 0.f;

    // Manipulate
    std::weak_ptr<GameObject> _manipulateObject;
    ImGuiHelper::DrawManipulateDesc _drawManipulateDesc; 
    bool _isUseManipulate = false;
    bool _isDrawedManipulate = false;
    bool _isMovedManipulate = false;
    bool _isUsingStart = false; 
    bool _isUsingEnd = false; 
    bool _isUsing = false;
    bool _isOver = false;

    //Vertex snap
    std::weak_ptr<MeshComponent> _weakClosestMeshComponent;
    BaseMesh* _manipulateBaseMesh = nullptr;
    BaseMesh* _closestBaseMesh = nullptr;
    bool      _isSnapping = false;

    // Camera Focused
    bool    _isFocusedCamera                = false;
    Vector3 _focusedCameraTargetPosition    = Vector3::Zero;
    Vector3 _focusedCameraStartPosition     = Vector3::Zero;
    float   _focusedLerpScale               = 0.1f;

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

        bool Execute() override;
        void Undo() override;
    };
    
protected:
    REFLECT_FIELDS_BEGIN(EditorTool)
    std::array<float, 3> CameraPosition{0, 0, 0};
    std::array<float, 4> CameraRotation{0, 0, 0, 1};
    float  CameraFov           = 70.f;
    float  CameraAspect        = 1.0f;
    float  CameraNearZ         = 0.01f;
    float  CameraFarZ          = 10000.f;
    float  CameraMoveSpeed     = 30.0f;
    float  CameraRotateSpeed   = 10.0f;
    float  CameraPivot         = 0.f;
    bool   VertexSnapUse       = false;
    float  VertexSnapThreshold = 100.f;
    bool   DrawGizmo           = true;
};
struct reflection_safe_ptr
{
    reflection_safe_ptr(EditorSceneTool* owner) { _owner = owner; }
    ~reflection_safe_ptr() = default;
    reflect_fields_struct*       operator->() { return Get(); }
    reflect_fields_struct&       operator*() { return *Get(); }
    const reflect_fields_struct* operator->() const { return Get(); }
    const reflect_fields_struct& operator*() const { return *Get(); }
    reflect_fields_struct*       Get()
    {
        if (_reflection == nullptr)
        {
            _reflection = reinterpret_cast<EditorSceneTool::reflect_fields_struct*>(_owner->get_reflect_fields());
        }
        return _reflection;
    }
    const reflect_fields_struct* Get() const
    {
        if (_reflection == nullptr)
        {
            _reflection = reinterpret_cast<EditorSceneTool::reflect_fields_struct*>(_owner->get_reflect_fields());
        }
        return _reflection;
    }

private:
    mutable EditorSceneTool::reflect_fields_struct* _reflection = nullptr;
    EditorSceneTool*                                _owner      = nullptr;
};
reflection_safe_ptr ReflectFields{this};

public:
virtual std::string SerializedReflectFields()
{
    serialized_reflect_event_recursive();
    return ReflectHelper::json::SerializedObjet(*ReflectFields);
}
virtual bool DeserializedReflectFields(std::string_view data)
{
    bool result = ReflectHelper::json::DeserializedObjet(*ReflectFields, data);
    deserialized_reflect_event_recursive();
    return result;
}

protected:
virtual void make_reflect_fields(void*& fields, unsigned long long& size)
{
    size_t size_of = sizeof(EditorSceneTool::reflect_fields_struct);
    fields         = malloc(size_of);
    size           = size_of;
    new (fields) EditorSceneTool::reflect_fields_struct();
}
virtual void serialized_reflect_event_recursive()
{
    Base::serialized_reflect_event_recursive();
    if constexpr (std::is_same_v<decltype(Base::SerializedReflectEvent),
                                 decltype(EditorSceneTool::SerializedReflectEvent)> == false)
    {
        EditorSceneTool::SerializedReflectEvent();
    }
}
virtual void deserialized_reflect_event_recursive()
{
    Base::deserialized_reflect_event_recursive();
    if constexpr (std::is_same_v<decltype(Base::DeserializedReflectEvent),
                                 decltype(EditorSceneTool::DeserializedReflectEvent)> == false)
    {
        EditorSceneTool::DeserializedReflectEvent();
    }
}
virtual void applyReflectFields(const std::function<void(std::string_view, void*)>& func)
{
    const auto view = rfl::to_view(*ReflectFields.Get());
    view.apply([&](auto& rflField) { func(rflField.name(), rflField.value()); });
}

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
        ReflectFields->CameraRotateSpeed, 
        ReflectFields->CameraPivot, 
        ReflectFields->CameraMoveSpeed
        )

    GETTER(bool, DrawGizmo)
    { 
        return ReflectFields->DrawGizmo;
    }
    SETTER(bool, DrawGizmo)
    {
        ReflectFields->DrawGizmo = value;      
    }
    PROPERTY(DrawGizmo)

    void UpdateCameraSetting();
    void UpdateReflectFields();
};

