#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Regen1 : public Token
    {
        TOKEN_DATA(205018, "재생Ⅰ")
        TOKEN_CONSTRUCTOR(Regen1, 50, 3, TokenTag::REGEN)

    private:
        void OnTurnStart(CharacterBase* owner) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        float HealFactor = 0.05f; // 최대 체력의 몇 퍼센트를 회복하는가
        REFLECT_FIELDS_END(Regen1)
    };
    class Regen2 : public Token
    {
        TOKEN_DATA(205019, "재생Ⅱ")
        TOKEN_CONSTRUCTOR(Regen2, 50, 3, TokenTag::REGEN)

    private:
        void OnTurnStart(CharacterBase* owner) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        float HealFactor = 0.1f;
        REFLECT_FIELDS_END(Regen2)
    };
    class Regen3 : public Token
    {
        TOKEN_DATA(205020, "재생Ⅲ")
        TOKEN_CONSTRUCTOR(Regen3, 50, 3, TokenTag::REGEN)

    private:
        void OnTurnStart(CharacterBase* owner) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        float HealFactor = 0.15f;
        REFLECT_FIELDS_END(Regen3)
    };
} // namespace TokenObject