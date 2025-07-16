#include "pchScripts.h"
#include "TokenApplyAction.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnAction/TurnActionFactory.h>

REGISTER_TURN_ACTION(TokenApplyAction)

TokenApplyAction::TokenApplyAction() 
{

}

std::string_view TokenApplyAction::GetActionInfo()
{
    constexpr const char8_t* info = u8"대상에게 출혈III 2개 부여";
    return (const char*)info;
}

void TokenApplyAction::ImGuiDrawActionEditor() 
{

}

std::string_view TokenApplyAction::GetActionName()
{
    return (const char*)u8"대상에게 토큰 부여";
}

