#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    /// <summary>
    /// <para>기절 토큰:</para>
    /// <para>기절 토큰을 보유한 대상은 본인의 턴 시작 시 턴을 스킵한다.</para>
    /// <para>기절 최대치는 1로, 중첩되지 않는다.</para>
    /// </summary>
    class Stun : public Token
    {
        TOKEN_DATA(205002)
    private:
        bool CanAdd(CharacterBase* owner) const override;
        void OnTurnStart(CharacterBase* owner) override;
    };

    /// <summary>
    /// <para>기절 저항 토큰:</para>
    /// <para>기절 저항 토큰이 0이 되면, 기절 저항 토큰을 없애고 기절 토큰을 획득합니다.</para>
    /// <para>기절 부여 키워드를 가진 효과를 받으면 기절 저항 토큰이 감소합니다.</para>
    /// </summary>
    class StunResistance : public Token
    {
        TOKEN_DATA(205029)
    private:     
    };
} // namespace TokenObject