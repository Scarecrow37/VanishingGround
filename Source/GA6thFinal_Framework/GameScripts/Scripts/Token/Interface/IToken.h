#pragma once
class CharacterBase;

class IToken
{
protected:
    virtual void OnRoundStart(CharacterBase* owner) = 0;
    virtual void OnRoundEnd(CharacterBase* owner)   = 0;
    virtual void OnTurnStart(CharacterBase* owner)  = 0;
    virtual void OnTurnEnd(CharacterBase* owner)    = 0;
    virtual void OnHit(CharacterBase* owner)        = 0;

public:
    virtual UINT8       GetStackCount() const       = 0;
    virtual UINT8       GetMaxStackCount() const    = 0;
};