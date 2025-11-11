#include "pchScripts.h"
#include "WeaponDamageAction.h"
#include <WeaponSystem/WeaponSystem.h>
#include <Stats/Player/PlayerStats.h>
#include <Stats/Player/PlayerStatsComponent.h>

using namespace u8_literals;

REFLECT_FUNCTION(WeaponDamageAction)

REGISTER_TURN_ACTION(WeaponDamageAction)

WeaponDamageAction::WeaponDamageAction() 
{
    UpdateActionInfo();
}

const std::string& WeaponDamageAction::GetActionName()
{
    static std::string actionName = u8"QTE 종료시 무기 데미지 영구 증가"_c_str;
    return actionName;
}

const std::string& WeaponDamageAction::GetActionInfo()
{
    return _actionInfo;
}

void WeaponDamageAction::ImGuiDrawActionEditor() 
{
    ImGui::Text("Action");
    ImGuiDrawPropertys();
    ImGui::Separator();
    ImGui::Text("Condition");
    ImguiDrawConditionEditor();
}

void WeaponDamageAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void WeaponDamageAction::OnPlayerQTEResult(Player& player, const QTE::OverallResult& result) 
{ 
    if (TriggerType::QTE_END == ReflectFields->Trigger && EvaluateConditions())
    {
        WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
        if (weaponSystem)
        {
           WeaponStats& stats = weaponSystem->GetCurrentWeaponElement().Stats;

           stats.HitDamage += ReflectFields->Damage;
           stats.HitDamageMultiplier *= ReflectFields->DamageMultiplier;

           stats.CriticalDamage += ReflectFields->CriticalDamage;
           stats.CriticalDamageMultiplier *= ReflectFields->CriticalDamageMultiplier;

           const std::string& weaponName = stats.WeaponName;
           std::string msg = weaponName;
           msg += u8"의"_c_str;
           if (ReflectFields->Damage != 0)
           {
               msg += u8"데미지 "_c_str;
               msg += "[+";
               msg += std::to_string(ReflectFields->Damage);
               msg += "]";
           }
           if (ReflectFields->DamageMultiplier - 1.f > Mathf::Epsilon)
           {
               msg += "[x";
               msg += std::to_string(ReflectFields->DamageMultiplier);
               msg += "]";
           }
           if (ReflectFields->CriticalDamage != 0)
           {
               msg += u8" 치명타 데미지 "_c_str;
               msg += "[+";
               msg += std::to_string(ReflectFields->CriticalDamage);
               msg += "]";
           }
           if (ReflectFields->CriticalDamageMultiplier - 1.f > Mathf::Epsilon)
           {
               msg += "[x";
               msg += std::to_string(ReflectFields->CriticalDamageMultiplier);
               msg += "]";
           }
           UmLogger.Message(LogLevel::LEVEL_INFO, msg);
        }
    }

}

void WeaponDamageAction::UpdateActionInfo()
{
    std::string_view triggerInfo;
    switch (ReflectFields->Trigger)
    {
    case TriggerType::QTE_END:
        triggerInfo = u8"QTE 종료시"_c_str;
        break;
    default:
        break;
    }
    _actionInfo = triggerInfo;
    _actionInfo += u8"무기의 데미지 "_c_str;
    _actionInfo += "[+";
    _actionInfo += std::to_string(ReflectFields->Damage);
    _actionInfo += "]";
    _actionInfo += "[x";
    _actionInfo += std::to_string(ReflectFields->DamageMultiplier);
    _actionInfo += "]";

    _actionInfo += u8" 치명타 데미지 "_c_str;
    _actionInfo += "[+";
    _actionInfo += std::to_string(ReflectFields->CriticalDamage);
    _actionInfo += "]";
    _actionInfo += "[x";
    _actionInfo += std::to_string(ReflectFields->CriticalDamageMultiplier);
    _actionInfo += "]";

}