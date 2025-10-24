#include "pchScripts.h"
#include "TokenTypeCountAttackDamageAdditional.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "TurnSystem/TurnSystemHelper.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "Token/TokenSystem.h"

REGISTER_TURN_ACTION(TokenTypeCountAttackDamageAdditional)

REFLECT_FUNCTION(TokenTypeCountAttackDamageAdditional)

TokenTypeCountAttackDamageAdditional::TokenTypeCountAttackDamageAdditional()
{
    UpdateActionInfo();
}

TokenTypeCountAttackDamageAdditional::~TokenTypeCountAttackDamageAdditional() = default;

const std::string& TokenTypeCountAttackDamageAdditional::GetActionName()
{
    using namespace u8_literals;
    const static std::string name = u8"공격시 플레이어의 데미지 배율이 대상의 토큰 종류 갯수 만큼 증가"_c_str;
    return name;
}

const std::string& TokenTypeCountAttackDamageAdditional::GetActionInfo()
{
    return _actionInfo;
}

void TokenTypeCountAttackDamageAdditional::ImGuiDrawActionEditor() 
{
    using namespace u8_literals;
    ImGuiDrawPropertys();

    if (TurnSystemHelper::DrawTargetComboboxWithToolTip(ReflectFields->TokenCountTarget, true))
    {
        UpdateActionInfo();
    }

    ImguiDrawConditionEditor();
}

void TokenTypeCountAttackDamageAdditional::DeserializedReflectEvent() 
{
    UpdateActionInfo();
}

void TokenTypeCountAttackDamageAdditional::UpdateActionInfo()
{
    using namespace u8_literals;
    _actionInfo.clear();

    _actionInfo =  u8"공격시 플레이어의 데미지 배율이 "_c_str;
    _actionInfo += (const char*)TurnSystemHelper::GetTurnTargetToolTip(ReflectFields->TokenCountTarget).data();
    _actionInfo += u8"의 토큰 종류 갯수 만큼 증가"_c_str;
}

void TokenTypeCountAttackDamageAdditional::OnPlayerBattleCalculateDamageModifier(
    Player& attacker,
    PlayerStats& attackerStats,
    WeaponStats& weaponStats, 
    Enemy& target,
    EnemyStats& targetStats)
{
    if (EvaluateConditions())
    {
        int multiplier = 0;
        std::vector<CharacterBase*> targets = TurnSystemHelper::GetTargetCharacters(ReflectFields->TokenCountTarget);
        for (auto& target : targets)
        {
            auto& inventory = target->GetTokenInventory();
            multiplier += inventory.GetValidTokenCountByTag();
        }
        weaponStats.CriticalDamageMultiplier += static_cast<float>(multiplier);
    }
}