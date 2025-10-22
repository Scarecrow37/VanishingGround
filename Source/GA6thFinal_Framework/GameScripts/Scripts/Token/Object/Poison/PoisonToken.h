#pragma once
#include <Token/Token.h>

namespace TokenObject
{
    // 중독 토큰
    class Poison : public Token
    {
        TOKEN_DATA(205001)
    private:
        void OnRoundStart(CharacterBase* owner) override;
    };

    // 중독 부여 토큰
    class PoisonGrant : public Token
    {
        TOKEN_DATA(205004)
    private:
        void OnTurnEnd(CharacterBase* owner) override;
        void OnTakeDamage(CharacterBase* source, CharacterBase* dest, int& damage,
                          QTE::NoteResult* noteResult) override;
    };
} // namespace TokenObject