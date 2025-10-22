#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 집중 토큰
    class Focus : public Token
    {
        TOKEN_DATA(205017)
    private:
        void OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                       QTE::NoteResult& noteResult, Enemy& target,
                                           EnemyStats& targetStats) override;
    };
}