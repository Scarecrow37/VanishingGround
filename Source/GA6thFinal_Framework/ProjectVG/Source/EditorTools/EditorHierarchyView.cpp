#include "EditorHierarchyView.h"

EditorHierarchyView::EditorHierarchyView()
{
    SetLabel("HierarchyView");
    SetInitialDockSpaceArea(DockSpaceArea::LEFT);
}

EditorHierarchyView::~EditorHierarchyView()
{
}

void  EditorHierarchyView::OnStartGui()
{
    return void ();
}

void  EditorHierarchyView::OnPreFrame()
{
    return void ();
}

void  EditorHierarchyView::OnFrame()
{
    return void ();
}

void  EditorHierarchyView::OnPostFrame()
{
    return void ();
}

void EditorHierarchyView::OnPopup()
{
    if (ImGui::MenuItem("Close Window"))
    {
        // 창을 닫는 로직 (사용자 구현 필요)
    }
    if (ImGui::MenuItem("Settings"))
    {
        // 설정 창 열기
    }
}
