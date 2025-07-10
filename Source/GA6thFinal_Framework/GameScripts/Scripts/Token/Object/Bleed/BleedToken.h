#pragma once
#include <Token/Token.h>

class Bleed1Token : public Token
{
    TOKEN_DATA(16000, "출혈Ⅰ")
private:
    void OnTurnStart(CharacterBase* owner) override;
};

class Bleed2Token : public Token
{
    TOKEN_DATA(16001, "출혈Ⅱ")
private:
    void OnTurnStart(CharacterBase* owner) override;
};

class Bleed3Token : public Token
{
    TOKEN_DATA(16002, "출혈III")
private:
    void OnTurnStart(CharacterBase* owner) override;
};
