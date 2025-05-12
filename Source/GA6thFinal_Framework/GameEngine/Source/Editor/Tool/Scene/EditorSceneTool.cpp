#include "pch.h"
#include "EditorSceneTool.h"
#include "../..//DynamicCamera/EditorDynamicCamera.h"

EditorSceneTool::EditorSceneTool() 
    : _useSnap(false)
    , _manipulateOperation(ImGuizmo::TRANSLATE)
    , _manipulateMode(ImGuizmo::MODE::WORLD)
    , _tempMatrix(Matrix::Identity), _view(Matrix::Identity)
    , _camera(std::make_unique<EditorDynamicCamera>())
{
    SetLabel("Scene");
    SetDockLayout(DockLayout::UP);
}

EditorSceneTool::~EditorSceneTool()
{
}

void EditorSceneTool::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmRenderer.GetCamera("Editor");
    ASSERT((nullptr != camera), L"Camera is nullptr");

    _camera->SetTarget(camera);

    //_position = Vector3(0.f, 0.f, -100.f);
    //_camera->SetPosition(_position);
}

void EditorSceneTool::OnPreFrame()
{
    // 프레임에 대한 호버링 중이면 No_Move플래그를 설정
    if (true == _isHorverdScene)
    {
        SetWindowFlag(ImGuiWindowFlags_NoMove);
        //UmLogger.Log(1, "SceneTool is Hovered");
    }
    else
    {
        SetWindowFlag(ImGuiWindowFlags_None);
        //UmLogger.Log(1, "SceneTool is UnHovered");
    }
}

void EditorSceneTool::OnFrame()
{
    //ImVec2 windowSize = ImGui::GetWindowSize();
    float  width      = ImGui::GetWindowWidth();
    float  height     = ImGui::GetWindowHeight();
    _aspect           = width/ height;
    _camera->GetCamera()->SetupPerspective(_fovDegree, _aspect, _nearZ, _farZ);
    //ImGuizmo::SetDrawlist();

    //const Matrix& cameraView       = _camera->GetViewMatrix();
    //const Matrix& cameraProjection = _camera->GetProjectionMatrix();

    //ImGuizmo::DrawGrid(*cameraView.m, *cameraProjection.m, *Matrix::Identity.m, _farZ);
    //ImGuizmo::DrawCubes(*cameraView.m, *cameraProjection.m, *_tempMatrix.m, 1);

    //if (false == IsLock() && false == Global::editorModule->IsLock())
    //{
    //    ProcessViewManipulate();
    //    ProcessManipulate();
    //}
    auto handle = UmRenderer.GetRenderSceneImage("Editor");
    UmRenderer.GetCamera("Editor");

    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::Image((ImTextureID)handle.ptr, size);
}

void EditorSceneTool::OnPostFrame()
{
    
}

void EditorSceneTool::OnFocus()
{
    /*
    _camera->Update();
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
    */
}
    
void EditorSceneTool::ProcessMove()
{
    /*
    ImGuiIO& io          = ImGui::GetIO();
    float    delta       = UmTime.DeltaTime();
    float    moveSpeed   = _moveSpeed * delta;
    float    rotateSpeed = _rotateSpeed * delta;

    auto foward = -_rotation.Forward();
    auto right  = -_rotation.Right();
    auto up     = -_rotation.Up();

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

    const float deltaTime   = UmTime.DeltaTime();
    const float moveSpeed   = 30.f;
    const float mouseSpped  = 5.f;
    const float rotateSpeed = 2.f;

    if (GetAsyncKeyState('W') & 0x8001)
    {
        _position += -_camera->GetWorldMatrix().Forward() * moveSpeed * deltaTime;
    }
    if (GetAsyncKeyState('S') & 0x8001)
    {
        _position += -_camera->GetWorldMatrix().Backward() * moveSpeed * deltaTime;
    }
    if (GetAsyncKeyState('A') & 0x8001)
    {
        _position += _camera->GetWorldMatrix().Left() * moveSpeed * deltaTime;
    }
    if (GetAsyncKeyState('D') & 0x8001)
    {
        _position += _camera->GetWorldMatrix().Right() * moveSpeed * deltaTime;
    }

    if (GetAsyncKeyState('Q') & 0x8001)
    {
        _position += _camera->GetWorldMatrix().Up() * moveSpeed * deltaTime;
    }

    if (GetAsyncKeyState('E') & 0x8001)
    {
        _position -= _camera->GetWorldMatrix().Up() * moveSpeed * deltaTime;
    }

    if (GetAsyncKeyState(VK_LEFT) & 0x8001)
    {
        _rotation += Vector3(0.f, -rotateSpeed * deltaTime, 0.f);
    }
    z

        if (GetAsyncKeyState(VK_RIGHT) & 0x8001)
    {
        _rotation += Vector3(0.f, rotateSpeed * deltaTime, 0.f);
    }

    if (GetAsyncKeyState(VK_UP) & 0x8001)
    {
        _rotation += Vector3(-rotateSpeed * deltaTime, 0.f, 0.f);
    }

    if (GetAsyncKeyState(VK_DOWN) & 0x8001)
    {
        _rotation += Vector3(rotateSpeed * deltaTime, 0.f, 0.f);
    }

    if (GetAsyncKeyState(VK_RBUTTON) & 0x8001)
    {
        POINT point;
        GetCursorPos((POINT*)&point);

        static Vector2 prevPoint{};
        static Vector2 currPoint{};
        currPoint.x   = point.x;
        currPoint.y   = point.y;
        Vector2 delta = (currPoint - prevPoint);
        delta.Clamp({-1.f, -1.f}, {1.f, 1.f});
        prevPoint = currPoint;

        if (delta != XMVectorZero())
        {
            _rotation += Vector3(delta.y * deltaTime * mouseSpped, delta.x * deltaTime * mouseSpped, 0.f);
        }

        ScreenToClient(UmCore->App.GetHwnd(), &point);
        SetCursorPos(point.x, point.y);
    }

    _camera->SetPosition(_position);
    _camera->SetRotation(_rotation);
    _camera->Update();

    */
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

    _view = _camera->GetCamera()->GetViewMatrix();

    ImGuizmo::ViewManipulate(*_view.m, _setDistance, viewManipulatePos, viewManipulateSize, 0x10101010);

    // 각 성분 행렬만 추출
    Matrix     world = _view.Invert();
    Vector3    scale;
    Quaternion rotationQuat;
    Vector3    translation;
    world.Decompose(scale, rotationQuat, translation);

    // 회전 행렬
    //_rotation = Matrix::CreateFromQuaternion(rotationQuat);
}

void EditorSceneTool::ProcessManipulate() 
{
    float windowWidth  = ImGui::GetWindowWidth();
    float windowHeight = ImGui::GetWindowHeight();
    float windowLeft   = ImGui::GetWindowPos().x;
    float windowTop    = ImGui::GetWindowPos().y;

    ImGuizmo::SetRect(windowLeft, windowTop, windowWidth, windowHeight);

    auto&         camera           = _camera->GetCamera();
    const Matrix& cameraView       = camera->GetViewMatrix();
    const Matrix& cameraProjection = camera->GetProjectionMatrix();

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
