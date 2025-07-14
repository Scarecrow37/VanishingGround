#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Bleed1 : public Token
    {
        USING_PROPERTY(Bleed1)
        TOKEN_DATA(16000, "출혈Ⅰ")
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
        USING_PROPERTY(Bleed2)
        TOKEN_DATA(16001, "출혈Ⅱ")
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
        USING_PROPERTY(Bleed3)
        TOKEN_DATA(16002, "출혈Ⅲ")
        REFLECT_PROPERTY(ReflectFields->TickDamage)

    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage = 40;
        REFLECT_FIELDS_END(Bleed3)
    };
}