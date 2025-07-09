#pragma once

class CharacterBase;

class IToken
{
private:
    virtual void OnRoundStart(CharacterBase* owner) = 0;
    virtual void OnRoundEnd(CharacterBase* owner)   = 0;
    virtual void OnTurnStart(CharacterBase* owner)  = 0;
    virtual void OnTurnEnd(CharacterBase* owner)    = 0;
    virtual void OnHit(CharacterBase* owner)        = 0;

public:
    virtual int   GetTokenID() const    = 0;
    virtual UINT8 GetStackCount() const = 0;
    virtual UINT8 GetMaxStackCount() const = 0;

};

class Token : public IToken
{
public:
    Token() = default;
    virtual ~Token() = default;

public:
    virtual void OnRoundStart(CharacterBase* owner) override = 0;
    virtual void OnRoundEnd(CharacterBase* owner)   override = 0;
    virtual void OnTurnStart(CharacterBase* owner)  override = 0;
    virtual void OnTurnEnd(CharacterBase* owner)    override = 0;
    virtual void OnHit(CharacterBase* owner)        override = 0;
    virtual int  GetTokenID() const                 override = 0;

public:
    UINT8   GetStackCount() const override;
    UINT8   GetMaxStackCount() const override;
    void    ClearStack();
    void    SetStack(UINT8 count);
    void    AddStack(UINT8 count = 1);
    void    RemoveStack(UINT8 count = 1);
    void    SetMaxStackCount(UINT8 maxStack);

private:
    UINT8   _stackCount = 0;
    UINT8   _maxStackCount = UINT8_MAX;
};
