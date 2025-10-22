#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Regen1 : public Token
    {
        TOKEN_DATA(205018)
    private:
        void OnTurnStart(CharacterBase* owner) override;
    };
    class Regen2 : public Token
    {
        TOKEN_DATA(205019)

    private:
        void OnTurnStart(CharacterBase* owner) override;
    };
    class Regen3 : public Token
    {
        TOKEN_DATA(205020)

    private:
        void OnTurnStart(CharacterBase* owner) override;
    };
} // namespace TokenObject