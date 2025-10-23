#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    // 신속 토큰
    class Haste : public Token
    {
        void OnRoundStart(CharacterBase* owner) override;
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;
    };
    class Haste1 : public Haste
    {
        TOKEN_DATA(205011)
    };
    class Haste2 : public Haste
    {
        TOKEN_DATA(205012)
    };
    class Haste3 : public Haste
    {
        TOKEN_DATA(205013)
    };
} // namespace TokenObject