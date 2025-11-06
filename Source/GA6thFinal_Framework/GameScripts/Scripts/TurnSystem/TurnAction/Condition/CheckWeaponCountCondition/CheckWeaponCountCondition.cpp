#include "pchScripts.h"
#include "CheckWeaponCountCondition.h"
#include "WeaponSystem/WeaponSystem.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REGISTER_TURN_ACTION_CONDITION(CheckWeaponCountCondition)

REFLECT_FUNCTION(CheckWeaponCountCondition)

void CheckWeaponCountCondition::SetTurnCount(int value) 
{
    ReflectFields->TurnCount = std::clamp(value, 1, (int)WeaponSystem::EQUIP_WEAPONS_SIZE);
    UpdateInfo();
}

bool CheckWeaponCountCondition::Evaluate()
{
    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        int count = TurnCount;
        return count == turnMode->GetPlayerWeaponCounter();
    }
    return false;
}

void CheckWeaponCountCondition::DeserializedReflectEvent() 
{
    UpdateInfo();
}

void CheckWeaponCountCondition::DrawImguiEditor() 
{
    ImGuiDrawPropertys();
}

const std::string& CheckWeaponCountCondition::GetConditionInfo()
{
    return _conditionInfo;
}

void CheckWeaponCountCondition::UpdateInfo() 
{
    using namespace u8_literals;
    _conditionInfo.clear();
    int count = TurnCount;
    if (1 == count)
    {
        _conditionInfo = u8"첫번째"_c_str;
    }
    else if (2 == count)
    {
        _conditionInfo = u8"두번째"_c_str;
    }
    else if (3 == count)
    {
        _conditionInfo = u8"세번째"_c_str;
    }
    else if (4 == count)
    {
        _conditionInfo = u8"네번째"_c_str;
    }
    _conditionInfo += u8" 무기로 행동"_c_str;
}

bool CheckWeaponCountCondition::CheckEvaluate(CharacterBase* character)
{
    return Evaluate();
}
