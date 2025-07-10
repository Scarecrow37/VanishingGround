#pragma once
#include <Token/Token.h>

class Bleed1Token : public Token
{
    TOKEN_DATA(16000, "Bleed1Token")
private:
    void OnTurnStart(CharacterBase* owner) override;
};
