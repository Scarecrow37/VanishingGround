#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Poison : public Token
    {
        TOKEN_DATA(205001, "중독")
        TOKEN_CONSTRUCTOR(Poison, 50, 3, TokenTag::POISON)
        REFLECT_PROPERTY()
    private:
        void OnRoundStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 10;
        REFLECT_FIELDS_END(Poison)
    };
} // namespace TokenObject