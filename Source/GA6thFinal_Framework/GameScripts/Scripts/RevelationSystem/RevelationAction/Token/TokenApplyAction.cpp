#include "pchScripts.h"
#include "TokenApplyAction.h"
#include <RevelationSystem/RevelationSystem.h>

REGISTER_REVELATION_CLASS(TokenApplyAction)

TokenApplyAction::TokenApplyAction() 
    : 
    RevelationActionBase(u8"대상에게 토큰 부여")
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

void TokenApplyAction::Execute(CharacterBase* attacker, CharacterBase* target) 
{

}
