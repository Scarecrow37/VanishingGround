#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Vanish : public Token
    {
        TOKEN_DATA(205045)
    private:
        void OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                               int& damage) override;
        void OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                              int& damage) override;
    };
}
