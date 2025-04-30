#include "pch.h"
#include "EditorCommandTool.h"

EditorCommandTool::EditorCommandTool() 
{

}

EditorCommandTool::~EditorCommandTool() {}

void EditorCommandTool::OnTickGui() {}

void EditorCommandTool::OnStartGui() {}

void EditorCommandTool::OnEndGui() {}

void EditorCommandTool::OnPreFrame() {}

void EditorCommandTool::OnFrame()
{
    auto& undoBegin = UmCommandManager.UndoStackBegin();
    auto& undoEnd   = UmCommandManager.UndoStackEnd();

    for (auto itr = undoBegin; itr != undoEnd; ++itr)
    {
        auto& command = *itr;
        if (nullptr != command)
        {
            auto& name = command->GetName();
            ImGui::Selectable(name.c_str());
            ImGui::Separator();
        }
    }
}

void EditorCommandTool::OnPostFrame() {}

void EditorCommandTool::OnFocus() {}

void EditorCommandTool::OnPopup() {}
