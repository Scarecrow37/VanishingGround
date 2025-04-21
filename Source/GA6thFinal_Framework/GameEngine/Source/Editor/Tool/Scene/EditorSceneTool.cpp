#include "pch.h"
#include "EditorSceneTool.h"

EditorSceneTool::EditorSceneTool() 
    : _useSnap(false)
    , _manipulateOperation(ImGuizmo::TRANSLATE)
    , _manipulateMode(ImGuizmo::MODE::WORLD),
      _temMatrix(Matrix::Identity), _view(Matrix::Identity)
    , _cameraSpeed(10.f)
{
    SetLabel("Scene");
    SetDockLayout(DockLayout::UP);

    _camera = new Camera();
    _camera->SetupPerspective(_fovDegree, _aspect, _nearZ, _farZ);
    _camera->SetPosition(Vector3(0.f, 0.f, -10.f));
}

EditorSceneTool::~EditorSceneTool()
{
    delete _camera;
}

void EditorSceneTool::OnStartGui()
{

}

void EditorSceneTool::OnPreFrame()
{
    
}

static const float identityMatrix[16] = {1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f,
                                         0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f};


void EditorSceneTool::OnFrame()
{
    ImGuizmo::SetDrawlist();

    const Matrix& cameraView       = _camera->GetViewMatrix();
    const Matrix& cameraProjection = _camera->GetProjectionMatrix();

    ProcessInput();

    ImGuizmo::DrawGrid(*cameraView.m, *cameraProjection.m, identityMatrix, _farZ);
    ImGuizmo::DrawCubes(*cameraView.m, *cameraProjection.m, *_temMatrix.m, 1);

    ProcessViewManipulate();
    ProcessManipulate();
}

void EditorSceneTool::OnPostFrame()
{
    
}

void EditorSceneTool::OnFocus()
{}

void EditorSceneTool::ProcessInput() 
{
    float speed = _cameraSpeed * UmTime.deltaTime();


    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_W))
    {
        _camera->AddPosition(Vector3(0.f, 0.f, speed));
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_S))
    {
        _camera->AddPosition(Vector3(0.f, 0.f, -speed));
    }

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Q))
    {
        _camera->AddPosition(Vector3(0.f, speed, 0.f));
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_E))
    {
        _camera->AddPosition(Vector3(0.f, -speed, 0.f));
    }

    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_D))
    {
        _camera->AddPosition(Vector3(speed, 0.f, 0.f));
    }
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_A))
    {
        _camera->AddPosition(Vector3(-speed, 0.f, 0.f));
    }
}

void EditorSceneTool::ProcessViewManipulate() 
{
    float viewManipulateRight = ImGui::GetWindowPos().x + ImGui::GetWindowWidth();
    float viewManipulateTop   = ImGui::GetWindowPos().y;

    ImGuizmo::SetRect(
        ImGui::GetWindowPos().x,
        ImGui::GetWindowPos().y,
        ImGui::GetWindowWidth(),
        ImGui::GetWindowHeight()
    );

    ImGuizmo::ViewManipulate(
        *_view.m, 
        _setaDistance,
        ImVec2(viewManipulateRight - 128, viewManipulateTop),
        ImVec2(128, 128),
        0x10101010
    );

    _camera->SetRotation(_view.ToEuler());
    _camera->Update();
}

void EditorSceneTool::ProcessManipulate() 
{
    ImGuizmo::SetRect(
        ImGui::GetWindowPos().x,
        ImGui::GetWindowPos().y,
        ImGui::GetWindowWidth(),
        ImGui::GetWindowWidth()
    );

    const Matrix& cameraView       = _camera->GetViewMatrix();
    const Matrix& cameraProjection = _camera->GetProjectionMatrix();

    ImGuizmo::Manipulate(
        *cameraView.m,
        *cameraProjection.m,
        _manipulateOperation,
        _manipulateMode,
        *_temMatrix.m,
        NULL,
        _useSnap ? &_snap[0] : NULL
    );
}
