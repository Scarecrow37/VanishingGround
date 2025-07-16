#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    /// <summary>
    /// <para>충격 토큰 :</para>
    /// <para>충격 토큰이 n개(충격 저항 토큰에 영향을 받음)가 쌓이면 기절 토큰을 부여하고 해당 수만큼 충격 토큰을
    /// 제거한다.</para>
    /// </summary>
    class Shock : public Token
    {
        TOKEN_DATA(16006, "충격")
        TOKEN_CONSTRUCTOR(Shock, 50, 999)
    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        // 충격이 몇 개 쌓이면 기절 토큰을 부여할지
        int TransitionTriggerCount = 4;
        REFLECT_FIELDS_END(Shock)
    };
} // namespace TokenObject