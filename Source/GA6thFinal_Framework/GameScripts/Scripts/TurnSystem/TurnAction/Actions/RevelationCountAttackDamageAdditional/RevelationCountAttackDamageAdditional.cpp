#include "pchScripts.h"
#include "RevelationCountAttackDamageAdditional.h"
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Stats/Weapon/WeaponStats.h>
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>
#include "RevelationSystem/RevelationSystem.h"

REGISTER_TURN_ACTION(RevelationCountAttackDamageAdditional)
REFLECT_FUNCTION(RevelationCountAttackDamageAdditional)

RevelationCountAttackDamageAdditional::RevelationCountAttackDamageAdditional() 
{
    UpdateActionInfo();
}

RevelationCountAttackDamageAdditional::~RevelationCountAttackDamageAdditional() = default;

const std::string& RevelationCountAttackDamageAdditional::GetActionName()
{
    static const std::string name = (const char*)u8"보유 계시 당 데미지 증가";
    return name;
}

const std::string& RevelationCountAttackDamageAdditional::GetActionInfo()
{
    return _actionInfo;
}

void RevelationCountAttackDamageAdditional::DeserializedReflectEvent()
{
    UpdateActionInfo();
}

void RevelationCountAttackDamageAdditional::ImGuiDrawActionEditor()
{
    static ReflectHelper::ImGuiDraw::InputAutoSetting setting = [] {
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


void RevelationCountAttackDamageAdditional::OnPlayerBattleCalculateDamageModifier(
    Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target, EnemyStats& targetStats)
{
    if (true == EvaluateConditions())
    {
        if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
        {
            // 보유 계시 n개 당 데미지 증가
            int totalCount  = static_cast<int>(system->GetRevelationTableElements().size());
            int perCount    = RevelationPerCount;
            int multiply    = totalCount / perCount;
            if (multiply > 0)
            {
                int   additionalDamage                   = multiply * AdditionalDamage;
                int   additionalCriticalDamage           = multiply * AdditionalCriticalDamage;
                float additionalDamageMultiplier         = static_cast<float>(multiply) * AdditionalDamageMultiplier;
                float additionalCriticalDamageMultiplier = static_cast<float>(multiply) * AdditionalCriticalDamageMultiplier;
                
                std::string msg = std::format("[{} +{}, x{}] [{} +{}, x{}]{}",
                    (const char*)u8"플레이어의 데미지 ", 
                    additionalDamage,
                    additionalDamageMultiplier, 
                    (const char*)u8"플레이어의 치명타 데미지 ",
                    additionalCriticalDamage,
                    additionalCriticalDamageMultiplier, 
                    (const char*)u8" 증가");
                
                UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
                weaponStats.HitDamage += additionalDamage;
                weaponStats.HitDamageMultiplier *= additionalDamageMultiplier;
                weaponStats.CriticalDamage += additionalCriticalDamage;
                weaponStats.CriticalDamageMultiplier *= additionalCriticalDamageMultiplier;
            }
        }
    }
}


void RevelationCountAttackDamageAdditional::UpdateActionInfo()
{
    _actionInfo = std::format("{}{}{}",
        (const char*)u8"보유 계시 ",
        ReflectFields->RevelationPerCount,
        (const char*)u8"개당 ");

    _actionInfo += std::format("[{} +{}, x{}]",
        (const char*)u8"피해", 
        ReflectFields->AdditionalDamage,
        ReflectFields->AdditionalDamageMultiplier);

    _actionInfo += std::format(" [{} +{}, x{}]", 
        (const char*)u8"치명타 피해",
        ReflectFields->AdditionalCriticalDamage,
        ReflectFields->AdditionalCriticalDamageMultiplier);
}
