#pragma once
#include <Interface/ITriggerType.h>

class CharacterBase;

class IToken : public ITriggerType
{
private: // ITriggerType을(를) 통해 상속됨.
    virtual void OnCombatStart(CharacterBase* source) override                      = 0;
    virtual void OnRoundStart(CharacterBase* owner) override                        = 0;
    virtual void OnRoundEnd(CharacterBase* owner) override                          = 0;
    virtual void OnTurnStart(CharacterBase* owner) override                         = 0;
    virtual void OnTurnEnd(CharacterBase* owner) override                           = 0;
    virtual void OnHit(CharacterBase* owner) override                               = 0;
    virtual void OnDead(CharacterBase* owner) override                              = 0;
    virtual void OnKill(CharacterBase* source, CharacterBase* destination) override = 0;
    virtual void OnTokenAdded(CharacterBase* owner, int tokenID) override                        = 0;
    virtual void OnTokenRemoved(CharacterBase* owner, int tokenID) override                      = 0;

public:
    virtual UINT16  GetStackCount() const      = 0;
    virtual UINT16  GetMaxStackCount() const   = 0;
};