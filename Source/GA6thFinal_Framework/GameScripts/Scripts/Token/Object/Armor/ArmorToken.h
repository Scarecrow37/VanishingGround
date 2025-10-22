#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 갑옷 토큰
    class Armor : public Token
    {
        void OnTakeDamage(CharacterBase* source, CharacterBase* dest, int& damage,
                          QTE::NoteResult* noteResult) override;
    };
    class Armor1 : public Token
    {
        TOKEN_DATA(205005)
    };
    class Armor2 : public Token
    {
        TOKEN_DATA(205006)
    };
    class Armor3 : public Token
    {
        TOKEN_DATA(205007)
    };
} // namespace TokenObject