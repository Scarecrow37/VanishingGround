#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Bleed1 : public Token
    {
        TOKEN_DATA(16000, "출혈Ⅰ")
        TOKEN_CONSTRUCTOR(Bleed1, 50, 3)
        REFLECT_PROPERTY(ReflectFields->TickDamage)

    private:
        void OnTurnStart(CharacterBase* owner) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 10;
        REFLECT_FIELDS_END(Bleed1)
    };

    class Bleed2 : public Token
    {
        TOKEN_DATA(16001, "출혈Ⅱ")
        TOKEN_CONSTRUCTOR(Bleed2, 50, 7)
        REFLECT_PROPERTY(ReflectFields->TickDamage)

    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 20;
        REFLECT_FIELDS_END(Bleed2)
    };

    class Bleed3 : public Token
    {
        TOKEN_DATA(16002, "출혈Ⅲ")
        TOKEN_CONSTRUCTOR(Bleed3, 50, 999)
        REFLECT_PROPERTY(ReflectFields->TickDamage)

    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 40;
        REFLECT_FIELDS_END(Bleed3)
    };
}