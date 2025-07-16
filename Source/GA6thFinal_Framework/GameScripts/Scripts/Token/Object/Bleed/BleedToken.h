#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Bleed1 : public Token
    {
        TOKEN_DATA(16000, "출혈Ⅰ")
        TOKEN_CONSTRUCTOR(Bleed1, 50, 3)
        REFLECT_PROPERTY(
            ReflectFields->TickDamage,
            ReflectFields->TransitionCount
        )

    private:
        void OnTurnStart(CharacterBase* owner) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage      = 10; // 매 턴마다 적용되는 데미지
        int TransitionCount = 4; // 다음 출혈로 전이되는 조건 수
        REFLECT_FIELDS_END(Bleed1)
    };

    class Bleed2 : public Token
    {
        TOKEN_DATA(16001, "출혈Ⅱ")
        TOKEN_CONSTRUCTOR(Bleed2, 50, 7)
        REFLECT_PROPERTY(
            ReflectFields->TickDamage,
            ReflectFields->TransitionCount
        )

    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage      = 20; // 매 턴마다 적용되는 데미지
        int TransitionCount = 8; // 다음 출혈로 전이되는 조건 수
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
        int TickDamage      = 40; // 매 턴마다 적용되는 데미지
        REFLECT_FIELDS_END(Bleed3)
    };
}