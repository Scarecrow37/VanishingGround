#include "pch.h"
#include "EditorModelHierarchy.h"
#include "Engine/GraphicsCore/BaseMesh.h"
#include "Engine/GraphicsCore/MeshRenderer.h"
#include "Engine/GraphicsCore/Model.h"

EditorModelHierarchy::EditorModelHierarchy() : _editorModelDetails(nullptr), _selectedMesh(nullptr)
{
    SetLabel("Hierarchy##model");
    SetDockLayout(ImGuiDir_Left);
}

void EditorModelHierarchy::OnTickGui() {}

void EditorModelHierarchy::OnStartGui()
{
    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("ModelDock");
    _editorModelDetails         = modelDock->GetGui<EditorModelDetails>();
}

void EditorModelHierarchy::OnEndGui() {}

void EditorModelHierarchy::OnPreFrameBegin() {}

void EditorModelHierarchy::OnPostFrameBegin() {}

void EditorModelHierarchy::OnFrameRender()
{
    auto&       meshRenderer = _editorModelDetails->_meshRenderer;
    const auto& model        = meshRenderer->GetModel();

    if (model)
    {
        char label[32] = "";
        char id[8]     = "";
        int  index     = 0;
        for (auto& mesh : model->GetMeshes())
        {
            sprintf_s(id, "##%d", index);
            strncpy_s(label, sizeof(label), mesh->GetName().data(), sizeof(label) - 1);
            strcat_s(label, sizeof(label), id);
            if (ImGui::Selectable(label, mesh.get() == _selectedMesh))
            {
                _selectedMesh = mesh.get();
                _editorModelDetails->SetSelectedMesh(index);
            }
            index++;
        }
    }
}

void EditorModelHierarchy::OnFrameClipped() {}

void EditorModelHierarchy::OnFrameEnd() {}

void EditorModelHierarchy::OnFrameFocusEnter() {}

void EditorModelHierarchy::OnFrameFocusStay() {}

void EditorModelHierarchy::OnFrameFocusExit() {}

void EditorModelHierarchy::OnFramePopupOpened() {}