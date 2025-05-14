#include "pch.h"
#include "EditorModelTool.h"
#include "Editor/DynamicCamera/EditorDynamicCamera.h"

EditorModelTool::EditorModelTool()
    : _camera(std::make_unique<EditorDynamicCamera>())
    , _editorModelDetails(nullptr)
{
    SetLabel("ModelViewer");  
    SetDockLayout(ImGuiDir_Left);
}

void EditorModelTool::OnTickGui()
{
}

void EditorModelTool::OnStartGui()
{
    std::shared_ptr<Camera> camera = UmRenderer.GetCamera("ModelViewer");
    ASSERT((nullptr != camera), L"Camera is nullptr");
    _camera->SetTarget(camera);

    SIZE size = UmCore->App.GetClientSize();
    camera->SetupPerspective(45.f, (float)size.cx / (float)size.cy, 0.1f, 1000.f);

    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("ModelDock");
    _editorModelDetails         = modelDock->GetGui<EditorModelDetails>();
}

void EditorModelTool::OnEndGui()
{
}

void EditorModelTool::OnPreFrameBegin()
{
}

void EditorModelTool::OnPostFrameBegin() {}

void EditorModelTool::OnFrameRender()
{
    auto handle = UmRenderer.GetRenderSceneImage("ModelViewer");

    ImVec2 size = ImGui::GetContentRegionAvail();

    ImGui::Image((ImTextureID)handle.ptr, size);
}

void EditorModelTool::OnFrameClipped() {}

void EditorModelTool::OnFrameEnd() {}

void EditorModelTool::OnFrameFocusEnter() {}

void EditorModelTool::OnFrameFocusStay() {}

void EditorModelTool::OnFrameFocusExit() {}

void EditorModelTool::OnFramePopupOpened() {}