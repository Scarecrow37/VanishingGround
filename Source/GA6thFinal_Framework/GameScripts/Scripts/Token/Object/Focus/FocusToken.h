#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 집중 토큰
    class Focus : public Token
    {
        TOKEN_DATA(205017)
    private:
        void OnTurnEnd(CharacterBase* owner) override;
        void OnPrePlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData) override;
    };
}