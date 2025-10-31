#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 중독 토큰
    class Poison : public Token
    {
        TOKEN_DATA(205001)
    private:
        bool CanAdd(CharacterBase* owner) const override;
        void OnRoundStart(CharacterBase* owner) override;
    };

    // 중독 부여 토큰
    class PoisonGrant : public Token
    {
        TOKEN_DATA(205004)
    private:
        void OnTurnEnd(CharacterBase* owner) override;
        void OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                               int& damage) override;
        void OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                              int& damage) override;
    };

    // 중독 저항 토큰
    class PoisonResistance : public Token
    {
        TOKEN_DATA(205028)
    };
} // namespace TokenObject