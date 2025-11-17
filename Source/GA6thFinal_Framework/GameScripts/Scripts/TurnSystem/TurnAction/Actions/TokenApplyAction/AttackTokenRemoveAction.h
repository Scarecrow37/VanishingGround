#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"

class AttackTokenRemoveAction : public TokenApplyAction
{
    USING_PROPERTY(AttackTokenRemoveAction)
public:
    AttackTokenRemoveAction()            = default;
    ~AttackTokenRemoveAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                               Enemy& target, EnemyStats& targetStats) override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};
