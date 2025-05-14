#include "FocusCommand.h"
Command::Hierarchy::FocusCommand::~FocusCommand() = default;
void Command::Hierarchy::FocusCommand::Execute()
{
    Super::Execute();
    EditorHierarchyTool::HierarchyFocusObjWeak = _newFocused;
    EditorSceneTool::SetManipulateObject(_newFocused);
}

void Command::Hierarchy::FocusCommand::Undo()
{
    Super::Undo();
    EditorHierarchyTool::HierarchyFocusObjWeak = _oldFocused;
    EditorSceneTool::SetManipulateObject(_oldFocused);
}

