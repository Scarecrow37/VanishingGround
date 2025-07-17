#include "pchScripts.h"
#include "TokenApplyAction.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnAction/TurnActionFactory.h>

REGISTER_TURN_ACTION(TokenApplyAction)

TokenApplyAction::TokenApplyAction() 
{

}

const std::string& TokenApplyAction::GetActionInfo()
{
    static const std::string info = (const char*)u8"대상에게 출혈III 2개 부여";
    return info;
}

void TokenApplyAction::ImGuiDrawActionEditor() 
{

}

const std::string& TokenApplyAction::GetActionName()
{
    static const std::string name = (const char*)u8"대상에게 토큰 부여";
    return name;
}

