#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Poison : public Token
    {
        TOKEN_DATA(205001)
    private:
        void OnRoundStart(CharacterBase* owner) override;
    };
} // namespace TokenObject