#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Bleed : public Token
    {
        TOKEN_DATA(205000, "출혈")
        TOKEN_CONSTRUCTOR(Bleed, 50, 3, TokenTag::BLEED)
        REFLECT_PROPERTY(
            ReflectFields->TickDamage,
            ReflectFields->TransitionCount
        )

    private:
        void OnRoundStart(CharacterBase* owner) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        int TickDamage      = 10; // 매 턴마다 적용되는 데미지
        int TransitionCount = 4;  // 다음 출혈로 전이되는 조건 수
        REFLECT_FIELDS_END(Bleed)
    };
}