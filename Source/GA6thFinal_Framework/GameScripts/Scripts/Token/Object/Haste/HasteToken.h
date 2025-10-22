#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    class Haste1 : public Token
    {
        TOKEN_DATA(205011, "신속Ⅰ")
        TOKEN_CONSTRUCTOR(Haste1, 50, 3, TokenTag::HASTE)

    private:

    public:
        REFLECT_FIELDS_BEGIN(Token)
        int SpeedIncrease = 1; // 속도 증가 수치
        REFLECT_FIELDS_END(Haste1)
    };
} // namespace TokenObject