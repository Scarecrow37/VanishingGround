#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 종말 예언 토큰
    class ProphecyDoom : public Token
    {
        TOKEN_DATA(205044)
    private:
        void OnTurnEnd(CharacterBase* owner) override;

    };
} // namespace TokenObject