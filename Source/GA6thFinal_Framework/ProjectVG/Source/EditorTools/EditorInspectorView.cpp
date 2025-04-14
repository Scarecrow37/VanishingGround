#include "EditorInspectorView.h"
#include "EditorLogsTool/EditorLogsTool.h"

EditorInspectorView::EditorInspectorView()
{
    SetLabel("InspectorView");
    SetInitialDockSpaceArea(DockSpaceArea::RIGHT);
}

EditorInspectorView::~EditorInspectorView()
{
}

void  EditorInspectorView::OnStartGui()
{
    return void();
}

void  EditorInspectorView::OnPreFrame()
{
    return void();
}

void  EditorInspectorView::OnFrame()
{
    if (false == _focusedObject.expired())
    {
        auto spFocusedObject = _focusedObject.lock();
        spFocusedObject->OnDrawInspectorView();
    }
}

void  EditorInspectorView::OnPostFrame()
{
    return void();
}

void EditorInspectorView::OnFocus()
{
}

void EditorInspectorView::TestGui()
{
    //ImGui::TextUnformatted("\xef\x85\x9b"); // FILE 아이콘 (FontAwesome 6)
    ImFont* font = ImGui::GetFont();
   
    if (ImGui::Button("Focus Logger"))
    {
        EditorLogsTool* tool = Global::editorManager->GetEditorObject<EditorLogsTool>();
        const char* label = tool->GetLabel().c_str();
        ImGui::SetWindowFocus(label);
    }
}