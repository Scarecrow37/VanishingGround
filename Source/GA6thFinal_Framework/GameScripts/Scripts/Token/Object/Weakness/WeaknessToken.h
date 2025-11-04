#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 약화 토큰
    class Weakness : public Token
    {
    private:
        void OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                               int& damage) override;
        void OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                              int& damage) override;
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