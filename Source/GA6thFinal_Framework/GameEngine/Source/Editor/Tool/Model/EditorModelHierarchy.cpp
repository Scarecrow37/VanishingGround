#include "pch.h"
#include "EditorModelHierarchy.h"
#include "GraphicsEngine/FBXConverter.h"
#include "GraphicsEngine/Skeleton.h"


EditorModelHierarchy::EditorModelHierarchy() : _editorModelDetails(nullptr), _selectedMesh(nullptr)
{
    SetLabel("Hierarchy##model");
    SetDockLayout(ImGuiDir_Left);
}

void EditorModelHierarchy::OnTickGui() {}

void EditorModelHierarchy::OnStartGui()
{
    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("Model##dock");
    _editorModelDetails         = modelDock->GetGui<EditorModelDetails>();
}

void EditorModelHierarchy::OnEndGui() {}

void EditorModelHierarchy::OnPreFrameBegin() {}

void EditorModelHierarchy::OnPostFrameBegin() {}

void EditorModelHierarchy::OnFrameRender()
{
    ShowMeshList();
    ImGui::Separator();
    ShowBoneList();
}

void EditorModelHierarchy::OnFrameClipped() {}

void EditorModelHierarchy::OnFrameEnd() {}

void EditorModelHierarchy::OnFrameFocusEnter() {}

void EditorModelHierarchy::OnFrameFocusStay() {}

void EditorModelHierarchy::OnFrameFocusExit() {}

void EditorModelHierarchy::OnFramePopupOpened() {}

void EditorModelHierarchy::ShowMeshList()
{
    if (ImGui::CollapsingHeader("Mesh List##model hierarchy", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const auto& meshRenderer = _editorModelDetails->_meshRenderer;
        if (meshRenderer)
        {
            const auto& model = meshRenderer->GetModel();
            if (model)
            {
                const auto& meshes = model->GetMeshes();
                for (int index = 0; index < meshes.size(); ++index)
                {
                    BaseMesh* mesh = meshes[index].get();
                    if (mesh)
                    {
                        ImGui::PushID(mesh);
                        const char* label      = mesh->GetName().data();
                        const bool  isSelected = mesh == _selectedMesh;
                        if (ImGui::Selectable(label, isSelected))
                        {
                            _selectedMesh = mesh;
                            _editorModelDetails->SetSelectedMesh(index);
                        }
                        ImGui::PopID();
                    }
                }
            }
            else
            {
                ImGui::Text("No model loaded.");
            }
        }
        else
        {
            ImGui::Text("No mesh renderer available.");
        }
    }
}

void EditorModelHierarchy::ShowBoneList() 
{
    if (ImGui::CollapsingHeader("Bone List##model hierarchy", ImGuiTreeNodeFlags_DefaultOpen))
    {
        const auto& meshRenderer = _editorModelDetails->_meshRenderer;
        if (meshRenderer)
        {
            const auto& model = meshRenderer->GetModel();
            if (model)
            {
                const auto& skeleton = model->GetSkeleton();
                if (skeleton)
                {
                    Bone& bone = skeleton->GetRootBone();
                    ShowBone(&bone);
                }
                else
                {
                    ImGui::Text("No skeleton available.");
                }
            }
            else
            {
                ImGui::Text("No model loaded.");
            }
        }
        else
        {
            ImGui::Text("No mesh renderer available.");
        }
    }
}



void EditorModelHierarchy::ShowBone(Bone* parent) 
{

    if (parent)
    {
        ImGui::PushID(parent);
        int  flags  = ImGuiTreeNodeFlags_None;
        flags = (parent == _selectedBone) ? flags | ImGuiTreeNodeFlags_Selected : flags;
        bool opened = ImGui::TreeNodeEx(parent->Name.c_str(), flags);

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            _selectedBone = parent;
        }
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("BoneContextMenu");
        }
        if (ImGui::BeginPopup("BoneContextMenu"))
        {
            if (ImGui::MenuItem("Copy Bone Name"))
            {
                File::CopyStrToClipBoard(parent->Name);
            }
            ImGui::EndPopup();
        }

        if (opened)
        {
            for (auto& child : parent->Children)
            {
                ShowBone(&child);
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }
}