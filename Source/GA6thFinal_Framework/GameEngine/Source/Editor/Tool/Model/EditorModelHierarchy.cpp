#include "pch.h"
#include "EditorModelHierarchy.h"
#include "Engine/GraphicsCore/MeshRenderer.h"
#include "Engine/GraphicsCore/Model.h"
#include "Engine/GraphicsCore/BaseMesh.h"

EditorModelHierarchy::EditorModelHierarchy()
    : _editorModelDetails(nullptr)
{
    SetLabel("Hierarchy##model");
    SetDockLayout(ImGuiDir_Left);
}

void EditorModelHierarchy::OnTickGui() {
}

void EditorModelHierarchy::OnStartGui()
{
    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("ModelDock");
    _editorModelDetails         = modelDock->GetGui<EditorModelDetails>();
}

void EditorModelHierarchy::OnEndGui() {
}

void EditorModelHierarchy::OnPreFrameBegin() {
}

void EditorModelHierarchy::OnPostFrameBegin() {}

void EditorModelHierarchy::OnFrameRender()
{
    auto& meshRenderer = _editorModelDetails->_meshRenderer;
    const auto& model = meshRenderer->GetModel();

    if (model)
    {
        for (auto& mesh : model->GetMeshes())
        {
            ImGui::Text(mesh->GetName().data());
            ImGui::Separator();
        }
    }
}

void EditorModelHierarchy::OnFrameClipped() {}

void EditorModelHierarchy::OnFrameEnd() {}

void EditorModelHierarchy::OnFrameFocusEnter() {}

void EditorModelHierarchy::OnFrameFocusStay() {}

void EditorModelHierarchy::OnFrameFocusExit() {}

void EditorModelHierarchy::OnFramePopupOpened() {}