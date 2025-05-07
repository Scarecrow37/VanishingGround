#include "pch.h"
#include "EditorDynamicCamera.h"

EditorDynamicCamera::EditorDynamicCamera()
    : _moveSpeed(10.f)
    , _rotationSpeed(5.f)
{
}

void EditorDynamicCamera::SetTarget(std::shared_ptr<Camera> camera)
{
    _camera = camera;
}

void EditorDynamicCamera::Update()
{
    const float deltaTime = UmTime.DeltaTime();

    ImGuiIO&    io          = ImGui::GetIO();
    float moveSpeed   = _moveSpeed * deltaTime;
    float rotateSpeed = _rotationSpeed * deltaTime;

    const Matrix& matrix = _camera->GetWorldMatrix();
    const Vector3 foward = -matrix.Forward();
    const Vector3 right  = -matrix.Right();
    const Vector3 up     = -matrix.Up();

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftShift))
    {
        moveSpeed *= 2.f;
    }
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
        _position += up * moveSpeed;
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_E))
    {
        _position += -up * moveSpeed;
    }


    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_MouseRight))
    {
        ImVec2 mouseDelta = io.MouseDelta;
        if (mouseDelta.x != 0.f || mouseDelta.y != 0.f)
        {
            float deltaX = mouseDelta.x * rotateSpeed;
            float deltaY = mouseDelta.y * rotateSpeed;
            _rotation += Vector3(deltaY, deltaX, 0.f);
        }
    }

    _camera->SetPosition(_position);
    _camera->SetRotation(_rotation);
}