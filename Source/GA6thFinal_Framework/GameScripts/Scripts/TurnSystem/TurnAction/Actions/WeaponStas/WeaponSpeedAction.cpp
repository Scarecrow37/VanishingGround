#include "pchScripts.h"
#include "WeaponSpeedAction.h"
#include <Stats/Player/PlayerStats.h>
#include <Stats/Player/PlayerStatsComponent.h>
#include <WeaponSystem/WeaponSystem.h>

using namespace u8_literals;

REFLECT_FUNCTION(WeaponSpeedAction)

REGISTER_TURN_ACTION(WeaponSpeedAction)

WeaponSpeedAction::WeaponSpeedAction()
{
    UpdateActionInfo();
}

const std::string& WeaponSpeedAction::GetActionName()
{
    static std::string actionName = u8"무기 속도 영구 증가"_c_str;
    return actionName;
}

const std::string& WeaponSpeedAction::GetActionInfo()
{
    return _actionInfo;
}

void WeaponSpeedAction::ImGuiDrawActionEditor()
{
    ImGui::Text("Action");
    ImGuiDrawPropertys();
    ImGui::Separator();
    ImGui::Text("Condition");
    ImguiDrawConditionEditor();
}

void WeaponSpeedAction::DeserializedReflectEvent()
{
    UpdateActionInfo();
}

void WeaponSpeedAction::OnPlayerQTEResult(Player& player, const QTE::OverallResult& result)
{
    WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
    if (weaponSystem)
    {
        if (TriggerType::QTE_END == ReflectFields->Trigger)
        {
            if (EvaluateConditions())
            {
                WeaponStats& stats = weaponSystem->GetCurrentWeaponElement().Stats;
                stats.Speed += Speed;

                const std::string& weaponName = stats.WeaponName;
                std::string        msg        = weaponName;
                msg += u8"의"_c_str;
                msg += u8"무기의 속도 "_c_str;
                msg += std::to_string(Speed);
                msg += u8" 증가"_c_str;
                UmLogger.Message(LogLevel::LEVEL_INFO, msg);
            }
        }
    }
}

void WeaponSpeedAction::OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon)
{
    WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
    if (weaponSystem)
    {
        if (TriggerType::ENEMY_DEAD_BY_WEAPON == ReflectFields->Trigger)
        {
            if (EvaluateConditions())
            {
                WeaponStats& stats = weaponSystem->GetCurrentWeaponElement().Stats;
                stats.Speed += Speed;

                const std::string& weaponName = stats.WeaponName;
                std::string        msg        = weaponName;
                msg += u8"의"_c_str;
                msg += u8"무기의 속도 "_c_str;
                msg += std::to_string(Speed);
                msg += u8" 증가"_c_str;
                UmLogger.Message(LogLevel::LEVEL_INFO, msg);
            }
        }
    }
}

void WeaponSpeedAction::UpdateActionInfo()
{
    std::string_view triggerInfo;
    switch (ReflectFields->Trigger)
    {
    case TriggerType::QTE_END:
        triggerInfo = u8"QTE 종료시"_c_str;
        break;
    case TriggerType::ENEMY_DEAD_BY_WEAPON:
        triggerInfo = u8"공격으로 적 처치시"_c_str;
        break;
    default:
        break;
    }
    _actionInfo = triggerInfo;
    _actionInfo += u8"무기의 속도 "_c_str;
    _actionInfo += std::to_string(Speed);
    _actionInfo += u8" 증가"_c_str;
}
