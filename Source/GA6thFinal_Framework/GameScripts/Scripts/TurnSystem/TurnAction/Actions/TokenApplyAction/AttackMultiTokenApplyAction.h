#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/MultiTokenApplyAction.h"

// 공격시 토큰 부여
class AttackMultiTokenApplyAction : public MultiTokenApplyAction
{
public:
    AttackMultiTokenApplyAction()               = default;
    ~AttackMultiTokenApplyAction() override = default;

protected:
    // TokenApplyAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                       WeaponStats& weaponStats, Enemy& target,
                                                       EnemyStats& targetStats) override;
private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};