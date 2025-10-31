#include "pchScripts.h"
#include "NotingAction.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"

REGISTER_TURN_ACTION(NotingAction)

const std::string& NotingAction::GetActionName()
{
    return name;
}

const std::string& NotingAction::GetActionInfo()
{
    return name;
}

void NotingAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    ImguiDrawConditionEditor();
}
