#pragma once
#include <Token/Token.h>

class Poison1Token : public Token
{
    USING_PROPERTY(Poison1Token)
    TOKEN_DATA(16003, "중독Ⅰ")
    REFLECT_PROPERTY()
private:
    void OnTurnStart(CharacterBase* owner) override;

private:
    REFLECT_FIELDS_BEGIN(Token)
    int TickDamage = 15;
    REFLECT_FIELDS_END(Poison1Token)
};

class Poison2Token : public Token
{
    USING_PROPERTY(Poison2Token)
    TOKEN_DATA(16004, "중독Ⅱ")
private:
    void OnTurnStart(CharacterBase* owner) override;

private:
    REFLECT_FIELDS_BEGIN(Token)
    int TickDamage = 30;
    REFLECT_FIELDS_END(Poison2Token)
};

class Poison3Token : public Token
{
    USING_PROPERTY(Poison3Token)
    TOKEN_DATA(16005, "중독Ⅲ")
private:
    void OnTurnStart(CharacterBase* owner) override;

private:
    REFLECT_FIELDS_BEGIN(Token)
    int TickDamage = 60;
    REFLECT_FIELDS_END(Poison3Token)
};
