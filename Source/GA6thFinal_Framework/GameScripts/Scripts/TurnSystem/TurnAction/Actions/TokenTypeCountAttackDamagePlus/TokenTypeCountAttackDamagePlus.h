#pragma once
#pragma once
#include "TurnSystem/TurnAction/TurnActionFactory.h"

class Player;
struct PlayerStats;
struct WeaponStats;
class Enemy;
struct EnemyStats;

// 공격시 특정 타입 토큰 갯수만큼 데미지 증가
class TokenTypeCountAttackDamagePlus : public TurnAction
{
    USING_PROPERTY(TokenTypeCountAttackDamagePlus)
public:
    TokenTypeCountAttackDamagePlus();
    ~TokenTypeCountAttackDamagePlus() override;

protected:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    GETTER(const std::string&, TokenTag) { return ReflectFields->TokenTag; }
    SETTER(const std::string&, TokenTag)
    {
        ReflectFields->TokenTag = value;
        UpdateActionInfo();
    }
    PROPERTY(TokenTag)

    GETTER(int, HitDamage)
    { 
        return ReflectFields->HitDamage;
    }
    SETTER(int, HitDamage)
    {
        ReflectFields->HitDamage = value;
        UpdateActionInfo();
    }
    PROPERTY(HitDamage)
  
    GETTER(int, CriticalDamage) { return ReflectFields->CriticalDamage; }
    SETTER(int, CriticalDamage)
    {
        ReflectFields->CriticalDamage = value;
        UpdateActionInfo();
    }
    PROPERTY(CriticalDamage)

    REFLECT_FIELDS_BEGIN(TurnAction)
    TurnTarget  TokenTarget    = TurnTarget::ENEMY;
    std::string TokenTag       = (const char*)u8"출혈";
    int         HitDamage      = 1;
    int         CriticalDamage = 1;
    REFLECT_FIELDS_END(TokenTypeCountAttackDamagePlus)

    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                               Enemy& target, EnemyStats& targetStats) override;

private:
    void TryTokenSystemInfoUpdate();
    bool _validTokenSystem = false;

    void        UpdateActionInfo();
    std::string _actionInfo;
};