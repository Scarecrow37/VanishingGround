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
                    std::vector<CharacterBase*> targetList = TurnSystemHelper::GetTargetCharacters(Target);
                    if (false == targetList.empty())
                    {
                        int recoveryHP = 0;
                        switch (RecoveryUnit)
                        {
                        case Unit::FLAT:
                            recoveryHP = ReflectFields->RecoveryHP;
                            break;
                        case Unit::PERCENT:
                            if (PlayerSystem* system = SingletonComponent<PlayerSystem>::GetInstance())
                            {
                                if (PlayerStatsComponent* statsComponent = system->GetPlayerStats())
                                {
                                    PlayerStats& stats = statsComponent->GetStats();
                                    float maxHP = static_cast<float>(stats.MaxHP);
                                    float raito = static_cast<float>(ReflectFields->RecoveryHP) / 100.f;
                                    float recoveryHPf = std::ceilf(maxHP * raito);
                                    recoveryHP = static_cast<int>(recoveryHPf);
                                }                                
                            }         
                            break;
                        default:
                            break;
                        }

                        for (auto& target : targetList)
                        {
                            if (target)
                            {
                                target->Heal(recoveryHP);
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

    TurnTarget target = ReflectFields->Target;
    std::string_view targetName = STR_NULL;
    targetName = (const char*)TurnSystemHelper::GetTurnTargetToolTip(target).data();
    _actionInfo = triggerName;
    _actionInfo += targetName;
    _actionInfo += (const char*)u8" 체력";
    _actionInfo += " ";
    std::string recoveryHP = std::to_string(ReflectFields->RecoveryHP);
    _actionInfo += recoveryHP;
    _actionInfo += (const char*)u8" 회복";
}