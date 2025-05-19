#include "pch.h"
#include "EditorDynamicCamera.h"

EditorDynamicCamera::EditorDynamicCamera() 
    : 
    _moveSpeed(10.f),
    _moveScale(1.f),
    _rotationSpeed(5.f) 
{}

void EditorDynamicCamera::SetTarget(std::shared_ptr<Camera> camera)
{
    _camera = camera;
}

void EditorDynamicCamera::Update()
{
    const float deltaTime = UmTime.DeltaTime();

    ImGuiIO& io          = ImGui::GetIO();
    float    moveSpeed   = _moveScale * _moveSpeed * deltaTime;
    float    rotateSpeed = _rotationSpeed * 0.001f;

    const Matrix& matrix = _camera->GetWorldMatrix();
    const Vector3 foward = -matrix.Forward();
    const Vector3 right  = -matrix.Right();
    const Vector3 up     = -matrix.Up();

    bool isRightClick = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_MouseRight);
    if (isRightClick)
    {
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

        ImVec2 mouseDelta = io.MouseDelta;
        if (mouseDelta.x != 0.f || mouseDelta.y != 0.f)
        {
            float deltaX = mouseDelta.x * rotateSpeed;
            float deltaY = mouseDelta.y * rotateSpeed;
            _rotation *= Quaternion::CreateFromAxisAngle(Vector3::Up, deltaX);
            _rotation = Quaternion::CreateFromAxisAngle(Vector3::Right, deltaY) * _rotation;
        }
        
        if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_MouseWheelY))
        {
            float wheel = io.MouseWheel;
            _moveScale += wheel * 0.01f;
            _moveScale = std::clamp(_moveScale, 0.f, 1000.f);
        }
    }

    _camera->SetPosition(_position);
    _camera->SetRotation(_rotation);
}