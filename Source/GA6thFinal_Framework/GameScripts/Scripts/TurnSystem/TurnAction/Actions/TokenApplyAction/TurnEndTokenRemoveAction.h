#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"

class TurnEndTokenRemoveAction : public TokenApplyAction
{
    USING_PROPERTY(TurnEndTokenRemoveAction)
public:
    TurnEndTokenRemoveAction()           = default;
    ~TurnEndTokenRemoveAction() override = default;

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
