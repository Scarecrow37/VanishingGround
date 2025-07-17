#include "pch.h"
#include "EditorDynamicCamera.h"

EditorDynamicCamera::EditorDynamicCamera() 
    : 
    _moveSpeed(10.f),
    _rotationSpeed(5.f), 
    _pivot(0.f),
    _isManipulated(false),
    _minmaxMoveSpeed(0.1f, 1000.f),
    _minmaxRotationSpeed(0.1f, 50.f)
{}

void EditorDynamicCamera::SetTarget(std::shared_ptr<Camera> camera)
{
    _camera = camera;
}

void EditorDynamicCamera::Update()
{
    ImGuiIO&      io           = ImGui::GetIO();
    const Vector3 forward      = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), _rotation);
    bool          isLeftAlt    = ImGui::IsKeyDown(ImGuiKey_LeftAlt);
    bool          isRightClick = ImGui::IsKeyDown(ImGuiKey_MouseRight);
    bool          isLeftClick  = ImGui::IsKeyDown(ImGuiKey_MouseLeft);

    if (isRightClick)
    {
        _isManipulated = UpdateMove();
        _isManipulated |= UpdateRotate();
        _pivotPosition = _position - forward * _pivot;
    }
    else
    {
        if (isLeftAlt && isLeftClick)
        {
            _isManipulated = UpdateRotate();
        }

        if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_MouseWheelY))
        {
            float wheel = io.MouseWheel;
            _pivot += wheel * _moveSpeed * 0.2f;
            _pivot = std::min(_pivot, 0.f);
            _isManipulated = true;
        }
        _position = _pivotPosition + forward * _pivot;
    }
    _camera->SetPosition(_position);
    _camera->SetRotation(_rotation);
}

bool EditorDynamicCamera::UpdateMove()
{
    bool  isMoved = false;
    const float deltaTime = UmTime.UnscaledDeltaTime();
    float moveSpeed = _moveSpeed * deltaTime;
    const Matrix& matrix = _camera->GetWorldMatrix();
    const Vector3 foward = -matrix.Forward();
    const Vector3 right  = -matrix.Right();
    const Vector3 up     = -matrix.Up();

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W))
    {
        _position += foward * moveSpeed; 
        isMoved = true;
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S))
    {
        _position += -foward * moveSpeed; 
        isMoved = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A))
    {
        _position += right * moveSpeed; 
        isMoved = true;
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D))
    {
        _position += -right * moveSpeed;
        isMoved = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Q))
    {
        _position += -Vector3::Up * moveSpeed;
        isMoved = true;
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_E))
    {
        _position += Vector3::Up * moveSpeed;
        isMoved = true;
    }

    return isMoved;
}

bool EditorDynamicCamera::UpdateRotate() 
{
    bool isMoved = false;
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mouseDelta = io.MouseDelta;
    if (mouseDelta.x != 0.f || mouseDelta.y != 0.f)
    {
        float rotateSpeed = _rotationSpeed * 0.001f;
        float deltaX = mouseDelta.x * rotateSpeed;
        float deltaY = mouseDelta.y * rotateSpeed;
        _rotation *= Quaternion::CreateFromAxisAngle(Vector3::Up, deltaX);
        _rotation = Quaternion::CreateFromAxisAngle(Vector3::Right, deltaY) * _rotation;
        isMoved   = true;
    }
    return isMoved;
}
