#include "pchScripts.h"
#include "RecoveryAction.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include "TurnSystem/TurnSystemHelper.h"
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Stats/CharacterStats.h>
#include "PlayerSystem/PlayerSystem.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "Stats/Player/PlayerStats.h"

REGISTER_TURN_ACTION(RecoveryAction)

REFLECT_FUNCTION(RecoveryAction)

using namespace u8_literals;

RecoveryAction::RecoveryAction() 
{
    UpdateActionInfo();
}

const std::string& RecoveryAction::GetActionName()
{
    static const std::string actionName = (const char*)u8"체력 회복";
    return actionName;
}

const std::string& RecoveryAction::GetActionInfo()
{
    return _actionInfo;
}

void RecoveryAction::ImGuiDrawActionEditor() 
{
    ImGui::Text("Action");
    ImGuiDrawPropertys();
    ImGui::Separator();
    ImGui::Text("Condition");
    ImguiDrawConditionEditor();
}

void RecoveryAction::OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                           WeaponStats& weaponStats, Enemy& target,
                                                           EnemyStats& targetStats)
{
    if (ReflectFields->Trigger == TriggerType::ATTACK)
    {
        if (EvaluateConditions())
        {
            ProcessHeal();
        }
    }
}

void RecoveryAction::OnEquipAccessory() 
{
    if (ReflectFields->Trigger == TriggerType::EQUIP_ACCESSORY)
    {
        if (EvaluateConditions())
        {
            ProcessHeal();
        }
    }
}

void RecoveryAction::OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon)
{
    if (ReflectFields->Trigger == TriggerType::WEAPON_KILL_ENEMY)
    {
        if (EvaluateConditions())
        {
            ProcessHeal();
        }
    }
}


void RecoveryAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void RecoveryAction::UpdateActionInfo()
{
    TriggerType trigger = Trigger;
    std::string_view triggerName = STR_NULL;
    switch (trigger)
    {
    case RecoveryAction::TriggerType::WEAPON_KILL_ENEMY:
        triggerName = (const char*)u8"무기 공격으로 적 처치시 ";
        break;
    case RecoveryAction::TriggerType::ATTACK:
        triggerName = (const char*)u8"공격할 때 마다 ";
        break;
    case RecoveryAction::TriggerType::EQUIP_ACCESSORY:
        triggerName = (const char*)u8"장신구로 장착시 ";
        break;
    default:
        break;
    }

    TurnTarget target = ReflectFields->Target;
    std::string_view targetName = STR_NULL;
    targetName = (const char*)TurnSystemHelper::GetTurnTargetToolTip(target).data();
    _actionInfo = triggerName;
    _actionInfo += targetName;
    _actionInfo += (const char*)u8" 체력";
    _actionInfo += " ";
    std::string recoveryHP = std::to_string(ReflectFields->RecoveryHP);
    _actionInfo += recoveryHP;
    if (ReflectFields->RecoveryUnit == Unit::PERCENT)
    {
        _actionInfo += (const char*)u8"퍼";
    }
    _actionInfo += (const char*)u8" 회복";
}

void RecoveryAction::ProcessHeal()
{
    std::vector<CharacterBase*> targetList = TurnSystemHelper::GetTargetCharacters(Target);
    if (false == targetList.empty())
    {
        for (auto& target : targetList)
        {
            ProcessHeal(target);
        }
    }
}

void RecoveryAction::ProcessHeal(CharacterBase* target)
{
    int recoveryHP = ReflectFields->RecoveryHP;
    if (target)
    {
        switch (ReflectFields->RecoveryUnit)
        {
        case Unit::FLAT:
            target->Heal(recoveryHP);
            break;
        case Unit::PERCENT:
            target->HealByPercentage(recoveryHP);
            break;
        default:
            break;
        }
    }
}

