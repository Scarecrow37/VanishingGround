#include "pchScripts.h"
#include "RecoveryAction.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Stats/CharacterStats.h>

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

void RecoveryAction::OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon)
{
    if (ReflectFields->Trigger == TriggerType::WEAPON_KILL_ENEMY)
    {
        if (EvaluateConditions())
        {
            if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
            {
                CombatStartPhase* combatStartPhase = turnMode->States->CombatStartPhase;
                if (combatStartPhase)
                {
                    static std::vector<CharacterBase*> targetList;
                    targetList.clear();
                    ActionTarget target    = ReflectFields->Target;
                    auto   lastAttaker     = Battle::GetLastAttacker().lock();
                    auto   lastTarget      = Battle::GetLastTarget().lock();
                    auto   lastTargetEnemy = Battle::GetLastTargetEnemy().lock();
                    switch (target)
                    {
                    default:
                    case ActionTarget::SELF: {
                        const auto& self = lastAttaker;
                        if (self)
                        {
                            targetList.push_back(self.get());
                        }
                        break;
                    }
                    case ActionTarget::PLAYER: {
                        Player* player = combatStartPhase->GetPlayer();
                        if (player && player == lastTarget.get())
                        {
                            targetList.push_back(player);
                        }
                        break;
                    }
                    case ActionTarget::ENEMY: {
                        if (lastTarget && lastTarget.get() == lastTargetEnemy.get())
                        {
                            targetList.push_back(lastTarget.get());
                        }
                        break;
                    }
                    case ActionTarget::ALL_ENEMIES: {
                        auto& enemys = combatStartPhase->GetEnemies();
                        for (auto& enemy : enemys)
                        {
                            targetList.push_back(enemy);
                        }
                        break;
                    }
                    case ActionTarget::ALL: {
                        auto& characters = combatStartPhase->GetCharacters();
                        for (auto& character : characters)
                        {
                            targetList.push_back(character);
                        }
                        break;
                    }
                    }

                    if (false == targetList.empty())
                    {
                        for (auto& target : targetList)
                        {
                            if (target)
                            {
                                target->Heal(ReflectFields->RecoveryHP);
                            }
                        }
                    }
                }
            }
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
    default:
        break;
    }

    ActionTarget target = ReflectFields->Target;
    std::string_view targetName = STR_NULL;
    switch (target)
    {
    case ActionTarget::SELF:
        targetName = u8"자신의"_c_str;
        break;
    case ActionTarget::PLAYER:
        targetName = u8"플레이어의"_c_str;
        break;
    case ActionTarget::ENEMY:
        targetName = u8"적의"_c_str;
        break;
    case ActionTarget::ALL_ENEMIES:
        targetName = u8"모든 적의"_c_str;
        break;
    case ActionTarget::ALL:
        targetName = u8"모든 캐릭터의"_c_str;
        break;
    default:
        break;
    }
    _actionInfo = triggerName;
    _actionInfo += targetName;
    _actionInfo += (const char*)u8" 체력";
    _actionInfo += " ";
    std::string recoveryHP = std::to_string(ReflectFields->RecoveryHP);
    _actionInfo += recoveryHP;
    _actionInfo += (const char*)u8" 회복";
}