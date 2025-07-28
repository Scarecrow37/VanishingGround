#include "pch.h"
#include "EditorModelInspector.h"

EditorModelInspector::EditorModelInspector()
{
    SetLabel("Inspector##model dock");
    SetDockLayout(ImGuiDir_Right);
}

void EditorModelInspector::OnPreFrameBegin() {}

void EditorModelInspector::OnPostFrameBegin() {}

void EditorModelInspector::OnFrameRender() {}

void EditorModelInspector::OnFrameClipped() {}

void EditorModelInspector::OnFrameEnd() {}

void EditorModelInspector::OnFrameFocusEnter() {}

void EditorModelInspector::OnFrameFocusStay() {}

void EditorModelInspector::OnFrameFocusExit() {}

void EditorModelInspector::OnFramePopupOpened() {}

void EditorModelInspector::SetSelectedObject(std::weak_ptr<IEditorObject> object)
{
}
