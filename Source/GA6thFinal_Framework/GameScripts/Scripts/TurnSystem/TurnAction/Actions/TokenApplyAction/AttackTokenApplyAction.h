#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"

//공격시 토큰 부여
class AttackTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(AttackTokenApplyAction)
public:
    AttackTokenApplyAction();
    ~AttackTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                               Enemy& target, EnemyStats& targetStats) override;
private:
    void UpdateActionInfo() override;
    std::string _actionInfo;
};