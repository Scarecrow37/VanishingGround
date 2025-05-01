#include "pch.h"
#include "EditorInspectorTool.h"

EditorInspectorTool::EditorInspectorTool()
{
    SetLabel("Inspector");
    SetDockLayout(DockLayout::RIGHT);

    SetWindowFlag(ImGuiWindowFlags_MenuBar); // 메뉴바 사용
}

EditorInspectorTool::~EditorInspectorTool()
{
}

void  EditorInspectorTool::OnStartGui()
{
}

void  EditorInspectorTool::OnPreFrame()
{
}

void  EditorInspectorTool::OnFrame()
{
    ShowMenuBarFrame();

    if (false == _focusedObject.expired())
    {
        auto spFocusedObject = _focusedObject.lock();
        spFocusedObject->OnInspectorStay();
    }
}

void  EditorInspectorTool::OnPostFrame()
{
}

void EditorInspectorTool::OnFocus()
{}

bool EditorInspectorTool::SetFocusObject(std::weak_ptr<IEditorObject> obj) 
{
    if (false == _isLockFocus)
    {
        _focusedObject = obj;
        return true;
    }
    return false;
}

bool EditorInspectorTool::IsFocused(std::weak_ptr<IEditorObject> obj)
{
    bool ownerIsExpired = _focusedObject.expired();
    bool objIsExpired   = obj.expired();
    if (false == ownerIsExpired && false == objIsExpired)
    {
        bool isSame = obj.lock() == _focusedObject.lock();
        return isSame;
    }
    return false;
}

void EditorInspectorTool::ShowMenuBarFrame()
{
    if (ImGui::BeginMenuBar())
    {
        ImGui::MenuItem("Lock", nullptr, &_isLockFocus);

        ImGui::EndMenuBar();
    }
}

namespace Command
{
    void FocusInspecor::Execute()
    {
        if (false == _newFocused.expired())
        {
            EditorInspectorTool::SetFocusObject(_newFocused);
        }
    }

    void FocusInspecor::Undo()
    {
        if (false == _oldFocused.expired())
        {
            EditorInspectorTool::SetFocusObject(_oldFocused);
        }
    }
}
