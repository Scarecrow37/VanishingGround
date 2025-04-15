#include "EditorInspectorView.h"
#include "EditorLogsTool/EditorLogsTool.h"

EditorInspectorView::EditorInspectorView()
{
    SetLabel("InspectorView");
    SetDockLayout(DockLayout::RIGHT);
}

EditorInspectorView::~EditorInspectorView()
{
}

void  EditorInspectorView::OnStartGui()
{
}

void  EditorInspectorView::OnPreFrame()
{
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
}

void EditorInspectorView::OnFocus()
{
}