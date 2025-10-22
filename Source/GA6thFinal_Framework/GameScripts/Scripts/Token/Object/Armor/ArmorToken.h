#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Armor1 : public Token
    {
        TOKEN_DATA(205005)

    private:
        void OnTakeDamage(CharacterBase* source, int& damage) override;
    };
    class Armor2 : public Token
    {
        TOKEN_DATA(205006)

    private:
        void OnTakeDamage(CharacterBase* source, int& damage) override;
    };
    class Armor3 : public Token
    {
        TOKEN_DATA(205007)

    private:
        void OnTakeDamage(CharacterBase* source, int& damage) override;
    };
} // namespace TokenObject