#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 검은 갑옷 토큰
    class BlackArmorToken : public Token
    {
        private:
        void OnHit(CharacterBase* owner) override;
    };
}
