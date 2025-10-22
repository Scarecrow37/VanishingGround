#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Armor1 : public Token
    {
        TOKEN_DATA(205005, "갑옷Ⅰ")
        TOKEN_CONSTRUCTOR(Armor1, 50, 3, TokenTag::ARMOR)

    private:
        void OnPreHitBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats, Enemy& target,
                                           EnemyStats& targetStats) override;
        void OnPreHitBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                           PlayerStats& targetStats) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        float DamageReductionFactor = 0.5f; // 받는 데미지 감소 비율
        REFLECT_FIELDS_END(Armor1)
    };
} // namespace TokenObject