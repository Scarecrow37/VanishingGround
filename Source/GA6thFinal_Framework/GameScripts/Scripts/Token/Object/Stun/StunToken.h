#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    class Stun : public Token
    {
        USING_PROPERTY(Stun)
        TOKEN_DATA(16007, "기절")
    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        REFLECT_FIELDS_END(Stun)
    };
} // namespace TokenObject