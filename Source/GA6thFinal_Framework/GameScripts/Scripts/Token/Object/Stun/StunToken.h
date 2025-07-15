#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    class Stun : public Token
    {
        TOKEN_DATA(16007, "기절")
        TOKEN_CONSTRUCTOR(Stun, 55, 1)
    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        REFLECT_FIELDS_END(Stun)
    };
} // namespace TokenObject