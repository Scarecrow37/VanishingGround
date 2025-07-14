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

    /// <summary>
    /// <para>충격 저항Ⅰ 토큰 :</para>
    /// <para>충격 토큰의 기절 전환 요구 개수를 n% 증가시킨다.
    /// 제거한다.</para>
    /// </summary>
    class ShockResistance1 : public Token
    {
        TOKEN_DATA(16008, "충격 저항Ⅰ")
        TOKEN_CONSTRUCTOR(ShockResistance1, 50, 999)

    private:
        void OnTurnStart(CharacterBase* owner) override;

    private:
        REFLECT_FIELDS_BEGIN(Token)
        // 충격이 몇 개 쌓이면 기절 토큰을 부여할지
        float ResistanceRate = 4;
        REFLECT_FIELDS_END(Shock)
    };

    /// <summary>
    /// <para>충격 저항Ⅱ 토큰 :</para>
    /// <para>충격 토큰의 기절 전환 요구 개수를 n% 증가시킨다.
    /// 제거한다.</para>
    /// </summary>
    class ShockResistance2 : public Token
    {
        USING_PROPERTY(ShockResistance2)
        REFLECT_FIELDS_BEGIN(Token)
        // 충격 저항률 1.0f = 100%
        inline static float ResistanceRate = 1.5f;
        REFLECT_FIELDS_END(ShockResistance2)
        TOKEN_DATA(16009, "충격 저항Ⅱ")

    private:
        void OnTurnStart(CharacterBase* owner) override;
    };

    /// <summary>
    /// <para>충격 저항Ⅲ 토큰 :</para>
    /// <para>충격 토큰의 기절 전환 요구 개수를 n% 증가시킨다.
    /// 제거한다.</para>
    /// </summary>
    class ShockResistance3 : public Token
    {
        USING_PROPERTY(ShockResistance3)
        REFLECT_FIELDS_BEGIN(Token)
        // 충격 저항률 1.0f = 100%
        inline static float ResistanceRate = 2.0f;
        REFLECT_FIELDS_END(ShockResistance3)
        TOKEN_DATA(16010, "충격 저항Ⅲ")

    private:
        void OnTurnStart(CharacterBase* owner) override;
    };
} // namespace TokenObject