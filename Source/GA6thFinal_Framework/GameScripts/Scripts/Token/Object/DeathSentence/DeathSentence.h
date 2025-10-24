#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 사형 선고 토큰
    class DeathSentence : public Token
    {
        TOKEN_DATA(205023)
    private:
        void OnRoundStart(CharacterBase* owner) override;
    };
} // namespace TokenObject