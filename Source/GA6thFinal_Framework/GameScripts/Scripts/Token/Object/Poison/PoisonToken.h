#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    class Poison1 : public Token
    {
        USING_PROPERTY(Poison1)
        TOKEN_DATA(16003, "중독Ⅰ")
        REFLECT_PROPERTY()
    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 15;
        REFLECT_FIELDS_END(Poison1)
    };

    class Poison2 : public Token
    {
        USING_PROPERTY(Poison2)
        TOKEN_DATA(16004, "중독Ⅱ")
    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 30;
        REFLECT_FIELDS_END(Poison2)
    };

    class Poison3 : public Token
    {
        USING_PROPERTY(Poison3)
        TOKEN_DATA(16005, "중독Ⅲ")
    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 60;
        REFLECT_FIELDS_END(Poison3)
    };
} // namespace TokenObject