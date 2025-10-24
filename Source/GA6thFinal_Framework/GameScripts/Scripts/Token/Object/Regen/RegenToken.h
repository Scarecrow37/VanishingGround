#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Regen : public Token
    {
        void OnTurnStart(CharacterBase* owner) override;
    };
    class Regen1 : public Regen
    {
        TOKEN_DATA(205018)
    };
    class Regen2 : public Regen
    {
        TOKEN_DATA(205019)
    };
    class Regen3 : public Regen
    {
        TOKEN_DATA(205020)
    };
} // namespace TokenObject