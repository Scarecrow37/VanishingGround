#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    class Bleed : public Token
    {
        TOKEN_DATA(205000)

    private:
        void OnRoundStart(CharacterBase* owner) override;

    public:
    };
}