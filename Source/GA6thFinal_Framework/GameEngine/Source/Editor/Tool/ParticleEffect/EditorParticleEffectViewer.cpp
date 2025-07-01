#include "pch.h"
#include "EditorParticleEffectViewer.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"


 EditorParticleEffectViewer::EditorParticleEffectViewer() 
         : _camera(std::make_unique<EditorDynamicCamera>())
 {
     SetLabel("Viewer##particleeffect");
     SetDockLayout(ImGuiDir_Up);

 }

 EditorParticleEffectViewer::~EditorParticleEffectViewer() {}

 void EditorParticleEffectViewer::OnTickGui()
{}

void EditorParticleEffectViewer::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmRenderer.GetCamera("ParticleEditor");
    GRAPHICS_ASSERT(nullptr != camera, L"Camera is nullptr");
    _camera->SetTarget(camera);
    _camera->SetPosition(Vector3(0.f, 0.f, -30.f));
    SIZE size = UmCore->App.GetClientSize();
    camera->SetupPerspective(45.f, (float)size.cx / (float)size.cy, 0.1f, 1000.f);

    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("ModelDock");
    //_editorModelDetails         = modelDock->GetGui<EditorModelDetails>();
}

void EditorParticleEffectViewer::OnEndGui()
{

}

void EditorParticleEffectViewer::OnPreFrameBegin()
{

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

void EditorParticleEffectViewer::OnFrameClipped()
{

}

void EditorParticleEffectViewer::OnFrameEnd()
{

}

void EditorParticleEffectViewer::OnFrameFocusEnter()
{

}

void EditorParticleEffectViewer::OnFrameFocusStay()
{
}

void EditorParticleEffectViewer::OnFrameFocusExit()
{

}

void EditorParticleEffectViewer::OnFrameRender()
{

    _camera->Update();



    auto handle = UmRenderer.GetRenderSceneImage("ParticleEditor");

    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::Image((ImTextureID)handle.ptr, size);
}

void EditorParticleEffectViewer::OnFramePopupOpened()
{

}

void EditorParticleEffectViewer::SerializedReflectEvent()
{

}

void EditorParticleEffectViewer::DeserializedReflectEvent()
{

}
