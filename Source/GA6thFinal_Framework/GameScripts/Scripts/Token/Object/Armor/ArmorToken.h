#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Armor1 : public Token
    {
        TOKEN_DATA(205005, "갑옷Ⅰ")
        TOKEN_CONSTRUCTOR(Armor1, 50, 3, TokenTag::ARMOR)

    private:
        void OnTakeDamage(CharacterBase* source, int& damage) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        float DamageReductionFactor = 0.5f; // 받는 데미지 감소 비율
        REFLECT_FIELDS_END(Armor1)
    };
    class Armor2 : public Token
    {
        TOKEN_DATA(205006, "갑옷Ⅱ")
        TOKEN_CONSTRUCTOR(Armor2, 50, 3, TokenTag::ARMOR)

    private:
        void OnTakeDamage(CharacterBase* source, int& damage) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        float DamageReductionFactor = 0.75f; // 받는 데미지 감소 비율
        REFLECT_FIELDS_END(Armor2)
    };
    class Armor3 : public Token
    {
        TOKEN_DATA(205007, "갑옷Ⅲ")
        TOKEN_CONSTRUCTOR(Armor3, 50, 3, TokenTag::ARMOR)

    private:
        void OnTakeDamage(CharacterBase* source, int& damage) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        float DamageReductionFactor = 1.0f; // 받는 데미지 감소 비율
        REFLECT_FIELDS_END(Armor3)
    };
} // namespace TokenObject