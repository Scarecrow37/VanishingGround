#pragma once
#include <Token/Token.h>
namespace TokenObject
{
    class Haste1 : public Token
    {
        TOKEN_DATA(205011, "신속Ⅰ")
        TOKEN_CONSTRUCTOR(Haste1, 50, 3, TokenTag::HASTE)

    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        int SpeedIncrease = 1; // 속도 증가 수치
        REFLECT_FIELDS_END(Haste1)
    };

    class Haste2 : public Token
    {
        TOKEN_DATA(205012, "신속Ⅱ")
        TOKEN_CONSTRUCTOR(Haste2, 50, 3, TokenTag::HASTE)

    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        int SpeedIncrease = 2; // 속도 증가 수치
        REFLECT_FIELDS_END(Haste2)
    };

    class Haste3 : public Token
    {
        TOKEN_DATA(205013, "신속Ⅲ")
        TOKEN_CONSTRUCTOR(Haste3, 50, 3, TokenTag::HASTE)

    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;

    public:
        REFLECT_FIELDS_BEGIN(Token)
        int SpeedIncrease = 3; // 속도 증가 수치
        REFLECT_FIELDS_END(Haste3)
    };
} // namespace TokenObject