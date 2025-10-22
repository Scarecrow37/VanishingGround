#pragma once

#include <Token/Token.h>

namespace TokenObject
{
    class Slow1 : public Token
    {
        TOKEN_DATA(205008)
    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;
    };
    class Slow2 : public Token
    {
        TOKEN_DATA(205008)
    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;
    };
    class Slow3 : public Token
    {
        TOKEN_DATA(205008)
    private:
        void OnRollRandomSpeed(CharacterBase* source, int& speed) override;
    };
} // namespace TokenObject