#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 분노 토큰
    class Rage : public Token
    {
        void OnTurnEnd(CharacterBase* owner) override;

        void OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                               int& damage) override;
        void OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                              int& damage) override;
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