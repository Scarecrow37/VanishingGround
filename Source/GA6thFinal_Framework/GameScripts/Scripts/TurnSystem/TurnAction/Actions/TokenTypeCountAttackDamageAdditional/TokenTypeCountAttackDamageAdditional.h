#pragma once
#include "TurnSystem/TurnAction/TurnActionFactory.h"

class Player;
struct PlayerStats;
struct WeaponStats;
class Enemey;
struct EnemyStats;

//공격시 토큰 갯수만큼 데미지 배율 증가
class TokenTypeCountAttackDamageAdditional : public TurnAction
{
    USING_PROPERTY(TokenTypeCountAttackDamageAdditional)
public:
    TokenTypeCountAttackDamageAdditional();
    ~TokenTypeCountAttackDamageAdditional() override;

protected:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void ImGuiDrawActionEditor() override;
    void DeserializedReflectEvent() override;

    REFLECT_FIELDS_BEGIN(TurnAction)
    TurnTarget TokenCountTarget = TurnTarget::SELF;
    REFLECT_FIELDS_END(TokenTypeCountAttackDamageAdditional)

    void OnPlayerBattleCalculateDamageModifier(
        Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats, Enemy& target,
        EnemyStats& targetStats) override;

private:
    void UpdateActionInfo();
    std::string _actionInfo;
};