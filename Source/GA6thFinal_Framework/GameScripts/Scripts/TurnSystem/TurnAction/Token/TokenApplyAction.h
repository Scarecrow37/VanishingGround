#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

class TokenApplyAction : public TurnAction
{
    USING_PROPERTY(TokenApplyAction)
public:
    TokenApplyAction();
    ~TokenApplyAction() override = default;
    REFLECT_PROPERTY()

    SETTER(int, TokenID) 
    { 
        ReflectFields->TokenID = std::max(value, 0); 
        UpdateActionInfo();
    }
    GETTER(int, TokenID) { return ReflectFields->TokenID; }
    // 부여할 토큰 ID
    PROPERTY(TokenID)

    SETTER(int, TokenCount)
    { 
        ReflectFields->TokenCount = std::max(value, 1); 
        UpdateActionInfo();
    }
    GETTER(int, TokenCount) { return ReflectFields->TokenCount; }
    // 부여할 토큰 개수
    PROPERTY(TokenCount)

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    int TokenID    = 16000;
    int TokenCount = 1;
    REFLECT_FIELDS_END(TokenApplyAction)

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;

    void DeserializedReflectEvent() override;

    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target,
                             EnemyStats& targetStats) override;

    void OnEnemyBattleCalculateDamageModifier(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                            PlayerStats& targetStats) override;

private:
    void UpdateActionInfo();
    std::string _actionInfo;
};