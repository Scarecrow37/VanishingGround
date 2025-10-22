#pragma once

#include <Token/Token.h>

namespace TokenObject
{
    class Slow : public Token
    {
    private:
        void OnRoundStart(CharacterBase* owner) override;
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;
    };
    class Slow1 : public Slow
    {
        TOKEN_DATA(205008)
    };
    class Slow2 : public Slow
    {
        TOKEN_DATA(205009)
    };
    class Slow3 : public Slow
    {
        TOKEN_DATA(205010)
    };
} // namespace TokenObject