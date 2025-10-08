#include "pch.h"
#include "EditorModelTool.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"

EditorModelTool::EditorModelTool()
    : _camera(std::make_unique<EditorDynamicCamera>())
    , _editorModelDetails(nullptr)
{
    SetLabel("Viewer##model");  
}

void EditorModelTool::OnTickGui()
{    
}

void EditorModelTool::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmGraphics.GetCamera("ModelViewer");
    assert(nullptr != camera && L"Camera is nullptr");
    _camera->SetTarget(camera);
    _camera->SetPosition(Vector3(0.f, 0.f, -5.f));
    SIZE size = UmCore->App.GetClientSize();
    camera->SetupPerspective(45.f, (float)size.cx / (float)size.cy, 0.1f, 1000.f);

    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("Model##dock");
    _editorModelDetails         = modelDock->GetGui<EditorModelDetails>();
}

void EditorModelTool::OnEndGui()
{
}

void EditorModelTool::OnPreFrameBegin()
{
}

void EditorModelTool::OnPostFrameBegin()
{
    ImVec2 windowPos  = ImGui::GetWindowPos();
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();

    auto clientLeft   = windowPos.x + contentMin.x;
    auto clientRight  = windowPos.x + contentMax.x;
    auto clientTop    = windowPos.y + contentMin.y;
    auto clientBottom = windowPos.y + contentMax.y;

    auto clientWidth  = clientRight - clientLeft;
    auto clientHeight = clientBottom - clientTop;
    auto aspect       = clientWidth / clientHeight;

    auto& camera = _camera->GetCamera();
    camera->SetupPerspective(45.f, aspect, 0.1f, 1000.f);
}

void EditorModelTool::OnFrameRender()
{
    auto handle = UmGraphics.GetRenderSceneImage("ModelViewer");

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 pos  = ImGui::GetCursorScreenPos();
    ImGui::Image((ImTextureID)handle.ptr, size);
    if (ImGui::IsWindowHovered() || IsFocusFrame())
    {
        if (_camera)
        {
            ImGui::SetCursorScreenPos(pos);
            float moveSpeed     = _camera->GetMoveSpeed();
            float rotationSpeed = _camera->GetRotationSpeed();
            int   pushCount     = 0;
            if (ImGui::IsKeyDown(ImGuiKey_MouseRight))
            {
                ImGuiStyle& style   = ImGui::GetStyle();
                ImVec4      bgCol   = style.Colors[ImGuiCol_FrameBg];
                ImVec4      textCol = style.Colors[ImGuiCol_Text];
                bgCol.w *= 0.5f;
                textCol.w *= 0.5f;
                ImGui::PushStyleColor(ImGuiCol_FrameBg, bgCol);
                ImGui::PushStyleColor(ImGuiCol_Text, textCol);
                ++pushCount;
                ++pushCount;
            }
            ImGui::SetNextItemWidth(150.0f);
            int flags = ImGuiSliderFlags_AlwaysClamp;
            ImGui::SliderFloat("Camera Move Speed##move speed", &moveSpeed, _camera->GetMinMoveSpeed(),
                               _camera->GetMaxMoveSpeed(), "%.2f", flags);
            ImGui::SetNextItemWidth(150.0f);
            ImGui::SliderFloat("Camera Rotation Speed##rotation speed", &rotationSpeed, _camera->GetMinRotationSpeed(),
                               _camera->GetMaxRotationSpeed(), "%.2f", flags);
            ImGui::PopStyleColor(pushCount);
            // 우클릭 + 마우스 휠 시 카메라 이동속도 높이기
            if (ImGui::IsKeyDown(ImGuiKey_MouseRight))
            {
                moveSpeed *= 1.0f + (ImGui::GetIO().MouseWheel * 0.05f);
            }
            _camera->SetMoveSpeed(moveSpeed);
            _camera->SetRotationSpeed(rotationSpeed);
            _camera->Update();
        }
    }
}

void EditorModelTool::OnFrameClipped() {}

void EditorModelTool::OnFrameEnd() {}

void EditorModelTool::OnFrameFocusEnter() {}

void EditorModelTool::OnFrameFocusStay() {}

void EditorModelTool::OnFrameFocusExit() {}

void EditorModelTool::OnFramePopupOpened() {}