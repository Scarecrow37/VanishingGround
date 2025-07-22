#include "pchScripts.h"
#include "CriticalDamageAction.h"
#include <RevelationSystem/RevelationSystem.h>
#include <TurnSystem/TurnAction/Condition/TokenCondition/TokenCondition.h>

#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Stats/Weapon/WeaponStats.h>
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>

REGISTER_TURN_ACTION(CriticalDamageAction)

CriticalDamageAction::CriticalDamageAction() 
{
    UpdateActionInfo();
}

CriticalDamageAction::~CriticalDamageAction() 
{

}

const std::string& CriticalDamageAction::GetActionInfo()
{ 
    return _actionInfo;
}

void CriticalDamageAction::ImGuiDrawActionEditor() 
{
    static ReflectHelper::ImGuiDraw::InputAutoSetting setting = [] 
    {
        ReflectHelper::ImGuiDraw::InputAutoSetting setting;
        setting._float.format = "%.1f";
        return setting;
    }();
    ImGui::Separator();
    ImGui::Text("Action");
    ImGuiDrawPropertys(setting);
    ImGui::Separator();
    ImGui::Text("Conditions");
    ImguiDrawConditionEditor();
}

void CriticalDamageAction::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void CriticalDamageAction::UpdateActionInfo() 
{
    _actionInfo = std::format("{}{}{}", (const char*)u8"치명타 피해 + ", ReflectFields->AdditionalDamage * 100.f, "%");
}


const std::string& CriticalDamageAction::GetActionName()
{
    static const std::string name = (const char*)u8"공격시 치명타 데미지 증가";
    return name;
}

void CriticalDamageAction::OnPlayerBattleStart(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                               Enemy& target, EnemyStats& targetStats)
{
    if (true == EvaluateConditions())
    {
        float       additionalDamage = AdditionalDamage;
        std::string msg = std::format("{}{}{}", (const char*)u8"플레이어의 치명타 데미지 ", additionalDamage * 100, (const char*)u8"증가");
        UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
        weaponStats.CriticalDamageMultiplier += additionalDamage;
    }
}

void CriticalDamageAction::OnEnemyBattleStart(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                              PlayerStats& targetStats)
{
    //적은 치명타가 없음.
}
