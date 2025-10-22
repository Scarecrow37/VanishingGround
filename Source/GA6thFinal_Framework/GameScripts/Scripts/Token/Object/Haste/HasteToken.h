#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    class Haste1 : public Token
    {
        TOKEN_DATA(205011)
    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;
    };

    class Haste2 : public Token
    {
        TOKEN_DATA(205012)
    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;
    };

    class Haste3 : public Token
    {
        TOKEN_DATA(205013)
    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;
    };
} // namespace TokenObject