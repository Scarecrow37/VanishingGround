#include "pch.h"
#include "EditorInspectorTool.h"

EditorInspectorTool::EditorInspectorTool()
{
    SetLabel("Inspector");
    SetDockLayout(ImGuiDir_Right);

    SetImGuiWindowFlag(ImGuiWindowFlags_MenuBar); // 메뉴바 사용
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
{
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

bool EditorInspectorTool::SetFocusObject(std::weak_ptr<IEditorObject> obj)
{
    if (false == _isLockFocus)
    {
        _focusedObject = obj;
        return true;
    }
    return false;
}

bool EditorInspectorTool::SetLockFocus(bool isLock)
{
    _isLockFocus = isLock;
    return true;
}

namespace Command
{
    namespace Inspector
    {
        void FocusObject::Execute()
        {
            if (false == _newFocused.expired())
            {
                EditorInspectorTool::SetFocusObject(_newFocused);
            }
        }

        void FocusObject::Undo()
        {
            if (false == _oldFocused.expired())
            {
                EditorInspectorTool::SetFocusObject(_oldFocused);
            }
        }

        void LockFocus::Execute() 
        {
            EditorInspectorTool::SetLockFocus(_isLock);
        }

        void LockFocus::Undo() 
        {
            EditorInspectorTool::SetLockFocus(!_isLock);
        }
    } // namespace Inspector
}
