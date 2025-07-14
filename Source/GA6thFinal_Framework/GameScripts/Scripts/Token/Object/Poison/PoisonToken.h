#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    class Poison1 : public Token
    {
        TOKEN_DATA(16003, "중독Ⅰ")
        TOKEN_CONSTRUCTOR(Poison1, 50, 3)
        REFLECT_PROPERTY()
    private:
        void OnEachTurnStart(CharacterBase* owner, CharacterBase* destiantion) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 15;
        REFLECT_FIELDS_END(Poison1)
    };

    class Poison2 : public Token
    {
        TOKEN_DATA(16004, "중독Ⅱ")
        TOKEN_CONSTRUCTOR(Poison2, 50, 3)
    private:
        void OnEachTurnStart(CharacterBase* owner, CharacterBase* destiantion) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 30;
        REFLECT_FIELDS_END(Poison2)
    };

    class Poison3 : public Token
    {
        TOKEN_DATA(16005, "중독Ⅲ")
        TOKEN_CONSTRUCTOR(Poison3, 50, 3)
    private:
        void OnEachTurnStart(CharacterBase* owner, CharacterBase* destiantion) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 60;
        REFLECT_FIELDS_END(Poison3)
    };
} // namespace TokenObject