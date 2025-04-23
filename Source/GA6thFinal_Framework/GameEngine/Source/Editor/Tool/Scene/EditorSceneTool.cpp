#include "pch.h"
#include "EditorSceneTool.h"

EditorSceneTool::EditorSceneTool() 
    : _useSnap(false)
    , _manipulateOperation(ImGuizmo::TRANSLATE)
    , _manipulateMode(ImGuizmo::MODE::WORLD)
    , _tempMatrix(Matrix::Identity), _view(Matrix::Identity)
{
    SetLabel("Scene");
    SetDockLayout(DockLayout::UP);
}

EditorSceneTool::~EditorSceneTool()
{
    delete _camera;
}

void EditorSceneTool::OnStartGui()
{
    _camera = new Camera();
    _position = Vector3(0.f, 0.f, -10.f);

    _camera->SetRotation(_rotation.ToEuler());
    _camera->SetPosition(_position);
    _camera->Update();
}

void EditorSceneTool::OnPreFrame()
{
    // 프레임에 대한 호버링 중이면 No_Move플래그를 설정
    if (true == _isHorverdScene)
    {
        SetWindowFlag(ImGuiWindowFlags_NoMove);
        //UmEngineLogger.Log(1, "SceneTool is Hovered");
    }
    else
    {
        SetWindowFlag(ImGuiWindowFlags_None);
        //UmEngineLogger.Log(1, "SceneTool is UnHovered");
    }
}

void EditorSceneTool::OnFrame()
{
    //_aspect = ImGui::GetWindowHeight() / ImGui::GetWindowWidth();
    _camera->SetupPerspective(_fovDegree, _aspect, _nearZ, _farZ);

    ImGuizmo::SetDrawlist();

    _camera->SetRotation(_rotation.ToEuler());
    _camera->SetPosition(_position);
    _camera->Update();

    const Matrix& cameraView       = _camera->GetViewMatrix();
    const Matrix& cameraProjection = _camera->GetProjectionMatrix();

    ImGuizmo::DrawGrid(*cameraView.m, *cameraProjection.m, *Matrix::Identity.m, _farZ);
    ImGuizmo::DrawCubes(*cameraView.m, *cameraProjection.m, *_tempMatrix.m, 1);

    if (false == IsLock() && false == Global::editorModule->IsLock())
    {
        ProcessViewManipulate();
        ProcessManipulate();
    }
}

void EditorSceneTool::OnPostFrame()
{
}

void EditorSceneTool::OnFocus()
{
    auto window     = ImGui::GetCurrentWindow();
    auto rect       = window->Rect();
    _isHorverdScene = ImGui::IsMouseHoveringRect(rect.Min, rect.Max);

    if (false == IsLock() && false == Global::editorModule->IsLock())
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            ProcessMove();
        }
        else
        {
            ProcessMode();
        }
    }
}

void EditorSceneTool::ProcessMove()
{
    ImGuiIO& io = ImGui::GetIO();
    float    delta = UmTime.deltaTime();
    float    moveSpeed = _moveSpeed * delta;
    float    rotateSpeed = _rotateSpeed * delta;

    auto foward = -_rotation.Forward();
    auto right = -_rotation.Right();
    auto up = -_rotation.Up();

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

    ImVec2 mouseDelta = io.MouseDelta;
    if (mouseDelta.x != 0.f || mouseDelta.y != 0.f)
    {
        float deltaX = mouseDelta.x * rotateSpeed;
        float deltaY = mouseDelta.y * rotateSpeed;
        _diretion += Vector3(deltaX, deltaY, 0.f);
        _rotation = XMMatrixRotationRollPitchYaw(_diretion.y, _diretion.x, 0.f);
    }
}

void EditorSceneTool::ProcessMode() 
{
    if (ImGui::IsKeyPressed(ImGuiKey_T))
        _manipulateOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E))
        _manipulateOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R))
        _manipulateOperation = ImGuizmo::SCALE;
}

void EditorSceneTool::ProcessViewManipulate() 
{
    float windowWidth  = ImGui::GetWindowWidth();
    float windowHeight = ImGui::GetWindowHeight();
    float windowLeft  = ImGui::GetWindowPos().x;
    float windowRight  = windowLeft + windowWidth;
    float windowTop    = ImGui::GetWindowPos().y;
    float windowBottom = windowTop + windowHeight;

    ImVec2 viewManipulateOffset = ImVec2(10, 10);
    ImVec2 viewManipulateSize   = ImVec2(128, 128);

    float viewManipulateRight = windowRight - viewManipulateSize.x + viewManipulateOffset.x;
    float viewManipulateTop   = windowTop + viewManipulateOffset.y;

    ImVec2 viewManipulatePos = ImVec2(viewManipulateRight, viewManipulateTop);

    ImGuizmo::SetRect(windowLeft, windowTop, windowWidth, windowHeight);

    _view = _camera->GetViewMatrix();

    ImGuizmo::ViewManipulate(*_view.m, _setDistance, viewManipulatePos, viewManipulateSize, 0x10101010);

    // 각 성분 행렬만 추출
    Matrix     world = _view.Invert();
    Vector3    scale;
    Quaternion rotationQuat;
    Vector3    translation;
    world.Decompose(scale, rotationQuat, translation);

    // 회전 행렬
    _rotation = Matrix::CreateFromQuaternion(rotationQuat);
}

void EditorSceneTool::ProcessManipulate() 
{
    float windowWidth  = ImGui::GetWindowWidth();
    float windowHeight = ImGui::GetWindowHeight();
    float windowLeft   = ImGui::GetWindowPos().x;
    float windowTop    = ImGui::GetWindowPos().y;

    ImGuizmo::SetRect(windowLeft, windowTop, windowWidth, windowHeight);

    const Matrix& cameraView       = _camera->GetViewMatrix();
    const Matrix& cameraProjection = _camera->GetProjectionMatrix();

    ImGuizmo::Manipulate(
        *cameraView.m,
        *cameraProjection.m,
        _manipulateOperation,
        _manipulateMode,
        *_tempMatrix.m,
        NULL,
        _useSnap ? &_snap[0] : NULL
    );
}
