#include "FocusCommand.h"

Command::Hierarchy::FocusCommand::~FocusCommand() = default;
bool Command::Hierarchy::FocusCommand::Execute()
{
    bool isFocused = EditorInspectorTool::IsFocusObject(_newFocused);
    // 이미 포커스된 오브젝트라면 아무것도 하지 않음
    if (true == isFocused)
    {   
        return false;
    }
    Super::Execute();
    EditorHierarchyTool::SetFocusObject(_newFocused);
    EditorSceneTool::SetManipulateObject(_newFocused);
    return true;
}

void Command::Hierarchy::FocusCommand::Undo()
{
    Super::Undo();
    EditorHierarchyTool::SetFocusObject(_oldFocused);
    EditorSceneTool::SetManipulateObject(_oldFocused);
}

