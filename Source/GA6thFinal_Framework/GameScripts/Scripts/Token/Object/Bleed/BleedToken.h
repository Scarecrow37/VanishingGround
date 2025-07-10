#pragma once
#include <Token/Token.h>

class Bleed1Token : public Token
{
    USING_PROPERTY(Bleed1Token)
    TOKEN_DATA(16000, "출혈Ⅰ")
private:
    void OnTurnStart(CharacterBase* owner) override;

private:
    REFLECT_FIELDS_BEGIN(Token)
    UINT16 BleedDamage = 10;
    REFLECT_FIELDS_END(Bleed1Token)
};

class Bleed2Token : public Token
{
    USING_PROPERTY(Bleed2Token)
    TOKEN_DATA(16001, "출혈Ⅱ")
private:
    void OnTurnStart(CharacterBase* owner) override;

private:
    REFLECT_FIELDS_BEGIN(Token)
    UINT16 BleedDamage = 20;
    REFLECT_FIELDS_END(Bleed2Token)
};

class Bleed3Token : public Token
{
    USING_PROPERTY(Bleed3Token)
    TOKEN_DATA(16002, "출혈Ⅲ")
private:
    void OnTurnStart(CharacterBase* owner) override;

private:
    REFLECT_FIELDS_BEGIN(Token)
    UINT16 BleedDamage = 40;
    REFLECT_FIELDS_END(Bleed3Token)
};