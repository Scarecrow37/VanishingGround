#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    class Poison1 : public Token
    {
        TOKEN_DATA(205001, "중독")
        TOKEN_CONSTRUCTOR(Poison1, 50, 3, TokenTag::POISON)
        REFLECT_PROPERTY()
    private:
        void OnEachTurnStart(CharacterBase* owner, CharacterBase * destination) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 15;
        REFLECT_FIELDS_END(Poison1)
    };
} // namespace TokenObject