#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Armor1 : public Token
    {
        TOKEN_DATA(205005, "갑옷Ⅰ")
        TOKEN_CONSTRUCTOR(Armor1, 50, 3, TokenTag::ARMOR)

    private:

    public:
        REFLECT_FIELDS_BEGIN(Token)
        REFLECT_FIELDS_END(Armor1)
    };
} // namespace TokenObject