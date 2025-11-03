#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

//연격 쌓이는거 막기
class ChainNotAdditionAction : public TurnAction
{
    USING_PROPERTY(ChainNotAdditionAction)
public:
    ChainNotAdditionAction() = default;
    ~ChainNotAdditionAction() override = default;

protected:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    inline static const std::string _name = (const char*)u8"연격이 쌓이지 않음";

    void OnPlayerBattleCalculateChainModifier(Player& attacker, PlayerStats& attackerStats,
                                                       WeaponStats& weaponStats, Enemy& target,
                                                       EnemyStats& targetStats) override;
};