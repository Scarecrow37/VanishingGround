#include "pch.h"
#include "EditorParticleEffectViewer.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"


 EditorParticleEffectViewer::EditorParticleEffectViewer() 
         : _camera(std::make_unique<EditorDynamicCamera>())
 {
     SetLabel("Viewer##particleeffect");
     SetDockLayout(ImGuiDir_Up);

 }

 EditorParticleEffectViewer::~EditorParticleEffectViewer() = default;

void EditorParticleEffectViewer::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmGraphics.GetCamera("ParticleEditor");
    assert(nullptr != camera && L"Camera is nullptr");
    _camera->SetTarget(camera);
    _camera->SetPosition(Vector3(0.f, 0.f, -10.f));
    _camera->SetMoveSpeed(1.f);
    SIZE size = UmCore->App.GetClientSize();
    camera->SetupPerspective(45.f, (float)size.cx / (float)size.cy, 0.1f, 1000.f);

    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("Effect##dock");
}
void EditorParticleEffectViewer::OnPostFrameBegin()
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

void EditorParticleEffectViewer::OnFrameRender()
{
    auto handle = UmGraphics.GetRenderSceneImage("ParticleEditor");

    ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 img_pos = ImGui::GetCursorScreenPos();
    ImGui::Image((ImTextureID)handle.ptr,size);

    // 이미지 위 특정 위치에 버튼 배치
    ImGui::SetCursorScreenPos(ImVec2(img_pos.x + 5, img_pos.y + 5));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 1)); 
    if (ImGui::Button("Reset Camera", { 100, 30 }) || ImGui::IsKeyPressed(ImGuiKey_R))
    {
        _camera->SetPosition(Vector3(0.f, 0.f, -30.f));
        _camera->SetRotation(Quaternion::Identity);
    }
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
    float camSpeed = _camera->GetMoveSpeed();
    if (ImGui::IsKeyPressed(ImGuiKey_C))
        camSpeed += 0.1f;
    if (ImGui::IsKeyPressed(ImGuiKey_V))
        camSpeed -= 0.1f;
    _camera->SetMoveSpeed(camSpeed);
    _camera->Update();
}