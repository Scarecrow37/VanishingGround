#include "pch.h"
#include "EditorSceneTool.h"

EditorSceneTool::EditorSceneTool()
{
    SetLabel("SceneView");
    SetDockLayout(DockLayout::UP);
}

EditorSceneTool::~EditorSceneTool()
{
}

void  EditorSceneTool::OnStartGui()
{
    return void ();
}

void  EditorSceneTool::OnPreFrame()
{
    return void ();
}

void DragDropFunc()
{
    ImGui::Text("Dragging");
}

void  EditorSceneTool::OnFrame()
{

}

void  EditorSceneTool::OnPostFrame()
{
    return void ();
}

void EditorSceneTool::OnFocus()
{
}
