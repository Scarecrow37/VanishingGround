#include "EditorSceneView.h"

EditorSceneView::EditorSceneView()
{
    SetLabel("SceneView");
    SetInitialDockSpaceArea(DockSpaceArea::UP);
}

EditorSceneView::~EditorSceneView()
{
}

void  EditorSceneView::OnStartGui()
{
    return void ();
}

void  EditorSceneView::OnPreFrame()
{
    return void ();
}

void DragDropFunc()
{
    ImGui::Text("Dragging");
}

void  EditorSceneView::OnFrame()
{
    ImGui::Text("Test Text");
    ImGui::Button("Test Button");
    ImGui::BeginChild("Child", ImVec2(200, 200), true, ImGuiWindowFlags_None);
    if (ImGui::TreeNodeEx("TreeNode", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Test Text");
        ImGui::Button("Test Button");
        ImGui::TreePop();
    }
    ImGui::EndChild();
    //static int a = 0;
    //static int b = 0;
    //
    //ImGui::Button("a", ImVec2(60, 60));
    //if (ImGuiHelper::DragDrop::SendDragDropEvent<int>("AtoB", &a, DragDropFunc))
    //{
    //}
    //if (ImGuiHelper::DragDrop::RecieveDragDropEvent<int>("BtoA", &a))
    //{
    //    Global::engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "DragDrop BtoA");
    //}
    //
    //ImGui::SameLine();
    //ImGui::Button("b", ImVec2(60, 60));
    //
    //if (ImGuiHelper::DragDrop::SendDragDropEvent<int>("BtoA", &b, DragDropFunc))
    //{
    //}
    //if (ImGuiHelper::DragDrop::RecieveDragDropEvent<int>("AtoB", &b))
    //{
    //    Global::engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "DragDrop AtoB");
    //}
}

void  EditorSceneView::OnPostFrame()
{
    return void ();
}

void EditorSceneView::OnFocus()
{
}
