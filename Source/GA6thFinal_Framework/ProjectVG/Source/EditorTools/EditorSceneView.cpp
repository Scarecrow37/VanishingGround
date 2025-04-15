#include "EditorSceneView.h"

EditorSceneView::EditorSceneView()
{
    SetLabel("SceneView");
    SetDockLayout(DockLayout::UP);
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
}

void  EditorSceneView::OnPostFrame()
{
    return void ();
}

void EditorSceneView::OnFocus()
{
}
