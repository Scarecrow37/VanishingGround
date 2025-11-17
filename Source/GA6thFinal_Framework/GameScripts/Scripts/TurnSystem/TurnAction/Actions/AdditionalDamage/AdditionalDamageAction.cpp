#include "pchScripts.h"
#include "AdditionalDamageAction.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnAction/Condition/TokenCondition/TokenCondition.h>

#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Stats/Weapon/WeaponStats.h>
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>

REGISTER_TURN_ACTION(AdditionalDamageAction)

REFLECT_FUNCTION(AdditionalDamageAction)

AdditionalDamageAction::AdditionalDamageAction() 
{
    UpdateActionInfo();
}

AdditionalDamageAction::~AdditionalDamageAction() 
{

}

const std::string& AdditionalDamageAction::GetActionInfo()
{ 
    return _actionInfo;
}

void AdditionalDamageAction::ImGuiDrawActionEditor() 
{
    static ReflectHelper::ImGuiDraw::InputAutoSetting setting = [] 
    {
        ReflectHelper::ImGuiDraw::InputAutoSetting setting;
        setting._float.format = "%.1f";
        return setting;
    }();
    ImGui::Text("Action");
    ImGuiDrawPropertys(setting);
    ImGui::Separator();
    ImGui::Text("Conditions");
    ImguiDrawConditionEditor();
}

void AdditionalDamageAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void AdditionalDamageAction::UpdateActionInfo() 
{
    _actionInfo = (const char*)u8"공격시 ";

    _actionInfo += std::format("[{} +{}, x{}]", (const char*)u8"피해", 
        ReflectFields->AdditionalDamage,
        ReflectFields->AdditionalDamageMultiplier);

    _actionInfo += std::format(" [{} +{}, x{}]", (const char*)u8"치명타 피해", 
        ReflectFields->AdditionalCriticalDamage,
        ReflectFields->AdditionalCriticalDamageMultiplier);
}


const std::string& AdditionalDamageAction::GetActionName()
{
    static const std::string name = (const char*)u8"공격시 데미지 증가";
    return name;
}

void AdditionalDamageAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                               Enemy& target, EnemyStats& targetStats)
{
    if (true == EvaluateConditions())
    {
        int   additionalDamage                   = AdditionalDamage;
        float additionalDamageMultiplier         = AdditionalDamageMultiplier;
        int   additionalCriticalDamage           = AdditionalCriticalDamage;
        float additionalCriticalDamageMultiplier = AdditionalCriticalDamageMultiplier;

        std::string msg = std::format("[{} +{}, x{}] [{} +{}, x{}]{}", 
            (const char*)u8"플레이어의 데미지 ", additionalDamage,additionalDamageMultiplier, 
            (const char*)u8"플레이어의 치명타 데미지 ",additionalCriticalDamage, additionalCriticalDamageMultiplier, 
            (const char*)u8" 증가");

        UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
        weaponStats.HitDamage += additionalDamage;
        weaponStats.HitDamageMultiplier *= additionalDamageMultiplier;
        weaponStats.CriticalDamage += additionalCriticalDamage;
        weaponStats.CriticalDamageMultiplier *= additionalCriticalDamageMultiplier;
    }
}

void AdditionalDamageAction::OnConvertWeaponViewModel(WeaponStats& stats) 
{
    //조건 0개면 항상 UI에 적용
    if (0 == ConditionCount)
    {
        int additionalDamage         = AdditionalDamage;
        int additionalCriticalDamage = AdditionalCriticalDamage;

        stats.HitDamage += additionalDamage;
        stats.CriticalDamage += additionalCriticalDamage;
    }
}
