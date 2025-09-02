#include "pch.h"
#include "EditorDynamicCamera.h"

EditorDynamicCamera::EditorDynamicCamera() 
    : 
    _moveSpeed(10.f),
    _rotationSpeed(5.f), 
    _pivot(0.f),
    _isMoved(false),
    _isRotated(false),
    _isSkipRotated(false),
    _isRightClickDown(false),
    _isHoveredWindow(false),
    _camera(nullptr),
    _position(Vector3::Zero),
    _rotation(Quaternion::Identity),
    _pivotPosition(Vector3::Zero),

    _minmaxMoveSpeed(0.1f, 1000.f),
    _minmaxRotationSpeed(0.1f, 50.f)
{}

void EditorDynamicCamera::SetTarget(std::shared_ptr<Camera> camera)
{
    _camera = camera;
}

void EditorDynamicCamera::Update(bool isHoveredWindow)
{
    _isMoved = false;
    _isRotated = false;
    _isHoveredWindow = isHoveredWindow;

    ImGuiIO&      io                    = ImGui::GetIO();
    const Vector3 forward               = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), _rotation);
    bool          isLeftAlt             = ImGui::IsKeyDown(ImGuiKey_LeftAlt);
    bool          isLeftClick           = ImGui::IsKeyDown(ImGuiKey_MouseLeft);
    bool          isRightClickPressed   = ImGui::IsKeyPressed(ImGuiKey_MouseRight, false);
    bool          isRightClickReleased  = ImGui::IsKeyReleased(ImGuiKey_MouseRight);

    if (_isRightClickDown)
    {
        _isMoved = UpdateMove();
        _isRotated  = UpdateRotate();
        _pivotPosition = _position - forward * _pivot;
        UpdateMouseCursor();
        // 우클릭 + 마우스 휠 시 카메라 이동속도 높이기
        if (ImGui::IsKeyDown(ImGuiKey_MouseRight))
        {
            float moveSpeed = _moveSpeed * (1.0f + (ImGui::GetIO().MouseWheel * 0.05f));
            SetMoveSpeed(moveSpeed);
        }
    }
    else
    {
        if (isHoveredWindow)
        {
            if (isLeftAlt && isLeftClick)
            {
                _isRotated = UpdateRotate();
            }

            if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_MouseWheelY))
            {
                float wheel = io.MouseWheel;
                _pivot += wheel;
                _pivot   = std::min(_pivot, 0.f);
                _isMoved = true;
            }
            _position = _pivotPosition + forward * _pivot;
        }
    }
    _camera->SetPosition(_position);
    _camera->SetRotation(_rotation);

    if (isRightClickPressed)
    {
        _isRightClickDown = true && _isHoveredWindow;
    }
    else if (isRightClickReleased)
    {
        _isRightClickDown = false;
    }
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
    if (true == _isSkipRotated)
    {
        _isSkipRotated = false;     
    }
    else
    {
        ImGuiIO& io         = ImGui::GetIO();
        ImVec2   mouseDelta = io.MouseDelta;
        if (mouseDelta.x != 0.f || mouseDelta.y != 0.f)
        {
            float rotateSpeed = _rotationSpeed * 0.001f;
            float deltaX      = mouseDelta.x * rotateSpeed;
            float deltaY      = mouseDelta.y * rotateSpeed;
            _rotation *= Quaternion::CreateFromAxisAngle(Vector3::Up, deltaX);
            _rotation = Quaternion::CreateFromAxisAngle(Vector3::Right, deltaY) * _rotation;
            isMoved   = true;
        }
    }
    return isMoved;
}

void EditorDynamicCamera::UpdateMouseCursor() 
{
    constexpr float windowPadding = 0.f;
    ImGuiIO&       io       = ImGui::GetIO();
    ImGuiViewport* viewport = ImGui::GetMainViewport();       // 메인 뷰포트를 가져옵니다.
    HWND           hWnd     = (HWND)viewport->PlatformHandle; // 뷰포트에서 직접 HWND를 얻습니다.
    if (hWnd)
    {
        ImVec2 windowPos  = ImGui::GetWindowPos();
        ImVec2 windowSize = ImGui::GetWindowSize();

        // 창의 경계 좌표 계산
        const float left   = windowPos.x + windowPadding;
        const float right  = windowPos.x + windowSize.x - windowPadding;
        const float top    = windowPos.y + windowPadding;
        const float bottom = windowPos.y + windowSize.y - windowPadding;

        ImVec2 newMousePos = io.MousePos;
        bool   teleported  = false;

        // 수평 래핑
        if (io.MousePos.x < left)
        {
            newMousePos.x = right - (left - io.MousePos.x);
            teleported    = true;
        }
        else if (io.MousePos.x > right)
        {
            newMousePos.x = left + (io.MousePos.x - right);
            teleported    = true;
        }

        // 수직 래핑
        if (io.MousePos.y < top)
        {
            newMousePos.y = bottom - (top - io.MousePos.y);
            teleported    = true;
        }
        else if (io.MousePos.y > bottom)
        {
            newMousePos.y = top + (io.MousePos.y - bottom);
            teleported    = true;
        }

        if (teleported)
        {
            // 화면 좌표로 변환.
            SetCursorPos((int)newMousePos.x, (int)newMousePos.y);

            // ImGui에게 새 위치를 알려줌
            io.MousePos = newMousePos;

            // 한 프레임 스킵
            _isSkipRotated = true;
        }
    } 
}
