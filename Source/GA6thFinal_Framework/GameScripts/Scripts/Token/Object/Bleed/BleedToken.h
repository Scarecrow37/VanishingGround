#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 출혈 토큰
    class Bleed : public Token
    {
        TOKEN_DATA(205000)
    private:
        bool CanAdd(CharacterBase* owner) const override;
        void OnTurnStart(CharacterBase* owner) override;
    };

    // 출혈 부여 토큰
    class BleedGrant : public Token
    {
        TOKEN_DATA(205003)
    private:
        void OnTurnEnd(CharacterBase* owner) override;
        void OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                               int& damage) override;
        void OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                              int& damage) override;
    };
   

    // 출혈 저항 토큰
    class BleedResistance : public Token
    {
        TOKEN_DATA(205027)
    };
}