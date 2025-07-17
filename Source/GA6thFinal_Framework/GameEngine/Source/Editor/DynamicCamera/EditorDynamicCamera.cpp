#include "pch.h"
#include "EditorDynamicCamera.h"

EditorDynamicCamera::EditorDynamicCamera() 
    : 
    _moveSpeed(10.f),
    _moveScale(1.f),
    _rotationSpeed(5.f), 
    _pivot(0.f)
{}

void EditorDynamicCamera::SetTarget(std::shared_ptr<Camera> camera)
{
    _camera = camera;
}

void EditorDynamicCamera::Update()
{
    ImGuiIO& io = ImGui::GetIO();
    const Vector3 forward = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), _rotation);
    bool isLeftAlt = ImGui::IsKeyDown(ImGuiKey_LeftAlt);
    bool isRightClick = ImGui::IsKeyDown(ImGuiKey_MouseRight);
    bool isLeftClick = ImGui::IsKeyDown(ImGuiKey_MouseLeft);

    if (isRightClick)
    {
        UpdateMove();
        UpdateRotate();
        if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_MouseWheelY))
        {
            float wheel = io.MouseWheel;
            _moveScale += wheel * 0.05f;
            _moveScale = std::clamp(_moveScale, 0.1f, 1000.f);
        }
        _pivotPosition = _position - forward * _pivot;
    }
    else
    {
        if (isLeftAlt && isLeftClick)
        {
            UpdateRotate();
        }

        if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_MouseWheelY))
        {
            float wheel = io.MouseWheel;
            _pivot += wheel;
            _pivot = std::min(_pivot, 0.f);
        }
        _position = _pivotPosition + forward * _pivot;
    }
    _camera->SetPosition(_position);
    _camera->SetRotation(_rotation);
}

void EditorDynamicCamera::UpdateMove() 
{
    const float deltaTime = UmTime.UnscaledDeltaTime();
    float moveSpeed = _moveScale * _moveSpeed * deltaTime;
    const Matrix& matrix = _camera->GetWorldMatrix();
    const Vector3 foward = -matrix.Forward();
    const Vector3 right  = -matrix.Right();
    const Vector3 up     = -matrix.Up();

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W))
    {
        _position += foward * moveSpeed;
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S))
    {
        _position += -foward * moveSpeed;
    }

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A))
    {
        _position += right * moveSpeed;
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D))
    {
        _position += -right * moveSpeed;
    }

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Q))
    {
        _position += -Vector3::Up * moveSpeed;
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_E))
    {
        _position += Vector3::Up * moveSpeed;
    }
}

void EditorDynamicCamera::UpdateRotate() 
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mouseDelta = io.MouseDelta;
    if (mouseDelta.x != 0.f || mouseDelta.y != 0.f)
    {
        float rotateSpeed = _rotationSpeed * 0.001f;
        float deltaX = mouseDelta.x * rotateSpeed;
        float deltaY = mouseDelta.y * rotateSpeed;
        _rotation *= Quaternion::CreateFromAxisAngle(Vector3::Up, deltaX);
        _rotation = Quaternion::CreateFromAxisAngle(Vector3::Right, deltaY) * _rotation;
    }
}
