#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 분노 토큰
    class Rage : public Token
    {
        void OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                              WeaponStats& weaponStats, QTE::NoteResult& noteResult, Enemy& target,
                                              EnemyStats& targetStats) override;
        void OnPreAttackBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                              PlayerStats& targetStats) override;
    };

    class Rage1 : public Rage
    {
        TOKEN_DATA(205021)
    };
    class Rage2 : public Rage
    {
        TOKEN_DATA(205022)
    };
} // namespace TokenObject