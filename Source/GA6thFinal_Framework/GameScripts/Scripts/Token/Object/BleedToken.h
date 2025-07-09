#pragma once
#include <Token/Token.h>

class BleedToken : public Token
{
    TOKEN_DATA(10001, "BleedToken")
private:
    // Token을(를) 통해 상속됨
    void OnRoundStart(CharacterBase* owner) override;
    void OnRoundEnd(CharacterBase* owner) override;
    void OnTurnStart(CharacterBase* owner) override;
    void OnTurnEnd(CharacterBase* owner) override;
    void OnHit(CharacterBase* owner) override;
};
