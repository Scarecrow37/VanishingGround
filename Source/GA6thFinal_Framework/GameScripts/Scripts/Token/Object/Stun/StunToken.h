#pragma once
#include <Token/Token.h>

class StunToken : public Token
{
    USING_PROPERTY(StunToken)
    TOKEN_DATA(16007, "기절")
private:
    void OnTurnStart(CharacterBase* owner) override;

private:
    REFLECT_FIELDS_BEGIN(Token)
    REFLECT_FIELDS_END(StunToken)
};
