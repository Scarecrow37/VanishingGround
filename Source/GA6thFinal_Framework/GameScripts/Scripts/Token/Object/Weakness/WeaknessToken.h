#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 약화 토큰
    class Weakness : public Token
    {
    private:
        void OnTurnEnd(CharacterBase* owner) override;
        void OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                              WeaponStats& weaponStats, QTE::NoteResult& noteResult, Enemy& target,
                                              EnemyStats& targetStats) override;
        void OnPreAttackBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                              PlayerStats& targetStats) override;
    };

    class Weakness1 : public Weakness
    {
        TOKEN_DATA(205024)
    };
    class Weakness2 : public Weakness
    {
        TOKEN_DATA(205025)
    };
    class Weakness3 : public Weakness
    {
        TOKEN_DATA(205026)
    };
}