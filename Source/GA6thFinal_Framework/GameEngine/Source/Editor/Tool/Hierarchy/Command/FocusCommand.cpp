#include "FocusCommand.h"

Command::Hierarchy::FocusCommand::~FocusCommand() = default;
void Command::Hierarchy::FocusCommand::Execute()
{
    Super::Execute();
    EditorHierarchyTool::SetFocusObject(_newFocused);
    EditorSceneTool::SetManipulateObject(_newFocused);
}

void Command::Hierarchy::FocusCommand::Undo()
{
    Super::Undo();
    EditorHierarchyTool::SetFocusObject(_oldFocused);
    EditorSceneTool::SetManipulateObject(_oldFocused);
}

