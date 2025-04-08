#include "EditorProjectMenu.h"

void EditorMenuScriptBuilder::OnSelected()
{
    Global::engineCore->ComponentFactory.InitalizeComponentFactory();
}
