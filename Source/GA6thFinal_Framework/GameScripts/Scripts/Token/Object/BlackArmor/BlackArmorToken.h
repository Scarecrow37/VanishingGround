#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 검은 갑옷 토큰
    class BlackArmor : public Token
    {
        TOKEN_DATA(205046)
    private:
        void OnHit(CharacterBase* owner) override;
    };
}
