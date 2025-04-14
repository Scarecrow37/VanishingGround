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
    static int a = 1;
    static int b = 0;

    ImGui::Button("a", ImVec2(60, 60));
    if (ImGuiHelper::DragDrop::SendDragDropEvent<int>("AtoB", &a, DragDropFunc))
    {
        Global::engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "AtoB Dragging");
    }
    if (ImGuiHelper::DragDrop::RecieveDragDropEvent<int>("BtoA", &a))
    {
        Global::engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "Drag BtoA Stop");
    }

    ImGui::SameLine();
    ImGui::Button("b", ImVec2(60, 60));
   
    if (ImGuiHelper::DragDrop::SendDragDropEvent<int>("BtoA", &b, DragDropFunc))
    {
        Global::engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "BtoA Dragging");
    }
    if (ImGuiHelper::DragDrop::RecieveDragDropEvent<int>("AtoB", &b))
    {
        Global::engineCore->EngineLogger.Log(LogLevel::LEVEL_DEBUG, "Drag AtoB Stop");
    }

    return void ();
}

void  EditorSceneView::OnPostFrame()
{
    return void ();
}

void EditorSceneView::OnFocus()
{
}
