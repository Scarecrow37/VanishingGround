#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Vanish : public Token
    {
        TOKEN_DATA(205045)
    private:
        void OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                              QTE::NoteResult& noteResult, Enemy& target,
                                              EnemyStats& targetStats) override;
        void OnPreAttackBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                              PlayerStats& targetStats) override;
    };
}
