#include "pchScripts.h"
#include "CriticalDamageAction.h"
#include <RevelationSystem/RevelationSystem.h>

REGISTER_REVELATION_CLASS(CriticalDamageAction)

CriticalDamageAction::CriticalDamageAction() 
    :
    RevelationActionBase(u8"치명타 데미지 증가") 
{
    UpdateActionInfo();
}

CriticalDamageAction::~CriticalDamageAction() 
{

}

std::string_view CriticalDamageAction::GetActionInfo()
{ 
    return _actionInfo;
}

void CriticalDamageAction::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
}

void RevelationActionBase::Execute(CharacterBase* attacker, CharacterBase* target) {}

void CriticalDamageAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void CriticalDamageAction::UpdateActionInfo() 
{
    _actionInfo = std::format("{}{}{}", (const char*)u8"치명타 피해 + ", ReflectFields->AdditionalDamage * 100.f, "%");
}
