#include "pchScripts.h"
#include "LastWeaponDamageCopy.h"
#include "WeaponSystem/WeaponSystem.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

REGISTER_TURN_ACTION(LastWeaponDamageCopy)

const std::string& LastWeaponDamageCopy::GetActionNameStr()
{
    using namespace u8_literals;
    const static std::string name = u8"이전 무기의 기본 피해량을 따라한다."_c_str;
    return name;
}

const std::string& LastWeaponDamageCopy::GetActionName()
{   
    return GetActionNameStr();
}

const std::string& LastWeaponDamageCopy::GetActionInfo()
{
    return GetActionNameStr();
}

void LastWeaponDamageCopy::ImGuiDrawActionEditor() 
{
    ImGuiDrawPropertys();
    ImguiDrawConditionEditor();
}

void LastWeaponDamageCopy::OnTurnStart(CharacterBase& destination) 
{
    if (WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance())
    {
        if (EvaluateConditions())
        {
            if (typeid(Player) == typeid(destination))
            {
                int lastSlot = system->LastWeaponSlot;
                if (0 <= lastSlot && lastSlot < system->EQUIP_WEAPONS_SIZE)
                {
                    WeaponElement& element   = system->GetCurrentWeaponElement();
                    WeaponStats&   currStats = element.Stats;
                    WeaponStats&   lastStats = system->GetWeaponStatsAtIndex(lastSlot);

                    currStats.HitDamage                = lastStats.HitDamage;
                    currStats.HitDamageMultiplier      = lastStats.HitDamageMultiplier;
                    currStats.CriticalDamage           = lastStats.CriticalDamage;
                    currStats.CriticalDamageMultiplier = lastStats.CriticalDamageMultiplier;
                }              
            }
        } 
    }  
}
