#pragma once
#include <Token/Token.h>

/// <summary>
/// <para>충격 토큰 :</para>
/// <para>충격 토큰이 n개(충격 저항 토큰에 영향을 받음)가 쌓이면 기절 토큰을 부여하고 해당 수만큼 충격 토큰을 제거한다.</para>
/// </summary>
class ShockToken : public Token
{
    USING_PROPERTY(ShockToken)
    REFLECT_FIELDS_BEGIN(Token)
    // 충격이 몇 개 쌓이면 기절 토큰을 부여할지
    inline static int TransitionTriggerCount = 4;
    REFLECT_FIELDS_END(ShockToken)
    TOKEN_DATA(16006, "충격")
private:
    void OnTurnStart(CharacterBase* owner) override;

};

/// <summary>
/// <para>충격 저항Ⅰ 토큰 :</para>
/// <para>충격 토큰의 기절 전환 요구 개수를 n% 증가시킨다.
/// 제거한다.</para>
/// </summary>
class ShockResistance1Token : public Token
{
    USING_PROPERTY(ShockResistance1Token)
    REFLECT_FIELDS_BEGIN(Token)
    // 충격 저항률 1.0f = 100%
    inline static float ResistanceRate = 1.0f;
    REFLECT_FIELDS_END(ShockResistance1Token)
    TOKEN_DATA(16008, "충격 저항Ⅰ")

private:
    void OnTurnStart(CharacterBase* owner) override;

};

/// <summary>
/// <para>충격 저항Ⅱ 토큰 :</para>
/// <para>충격 토큰의 기절 전환 요구 개수를 n% 증가시킨다.
/// 제거한다.</para>
/// </summary>
class ShockResistance2Token : public Token
{
    USING_PROPERTY(ShockResistance2Token)
    REFLECT_FIELDS_BEGIN(Token)
    // 충격 저항률 1.0f = 100%
    inline static float ResistanceRate = 1.5f;
    REFLECT_FIELDS_END(ShockResistance2Token)
    TOKEN_DATA(16009, "충격 저항Ⅱ")


private:
    void OnTurnStart(CharacterBase* owner) override;

};

/// <summary>
/// <para>충격 저항Ⅲ 토큰 :</para>
/// <para>충격 토큰의 기절 전환 요구 개수를 n% 증가시킨다.
/// 제거한다.</para>
/// </summary>
class ShockResistance3Token : public Token
{
    USING_PROPERTY(ShockResistance3Token)
    REFLECT_FIELDS_BEGIN(Token)
    // 충격 저항률 1.0f = 100%
    inline static float ResistanceRate = 2.0f;
    REFLECT_FIELDS_END(ShockResistance3Token)
    TOKEN_DATA(16010, "충격 저항Ⅲ")

private:
    void OnTurnStart(CharacterBase* owner) override;

};