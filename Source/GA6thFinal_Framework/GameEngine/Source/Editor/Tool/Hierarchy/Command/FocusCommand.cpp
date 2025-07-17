#include "FocusCommand.h"

Command::Hierarchy::FocusCommand::~FocusCommand() = default;
bool Command::Hierarchy::FocusCommand::Execute()
{
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

