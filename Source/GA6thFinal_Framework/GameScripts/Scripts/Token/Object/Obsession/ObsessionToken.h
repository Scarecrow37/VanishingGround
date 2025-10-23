#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 강박증 (플레이어만 적용)
    class Obsession : public Token
    {
    private:
        void OnTurnEnd(CharacterBase* owner) override;

        void OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                               int& damage) override;

    };

    class Obsession1 : public Obsession
    {
        TOKEN_DATA(205014)
    };
    class Obsession2 : public Obsession
    {
        TOKEN_DATA(205015)
    };
    class Obsession3 : public Obsession
    {
        TOKEN_DATA(205016)
    };
} // namespace TokenObject