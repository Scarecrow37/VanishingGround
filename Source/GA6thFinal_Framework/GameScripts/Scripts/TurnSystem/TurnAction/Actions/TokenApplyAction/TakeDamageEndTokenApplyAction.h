#pragma once
#include <TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h>

//플레이어 체력 잃을때 토큰 부여
class TakeDamageEndTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(TakeDamageEndTokenApplyAction)
public:
    TakeDamageEndTokenApplyAction() = default;
    ~TakeDamageEndTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;

private:
    void UpdateActionInfo() override;
    std::string _actionInfo;

protected:
    REFLECT_PROPERTY(ReflectFields->OnlyAttackDamage)

    REFLECT_FIELDS_BEGIN(TokenApplyAction)
    bool OnlyAttackDamage = false;
    REFLECT_FIELDS_END(TakeDamageEndTokenApplyAction)

    void OnPlayerTakeDamageEnd(Player& target, int damage) override;

    void OnEnemyBattleCalculateDamageModifier(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                              PlayerStats& targetStats) override;

};