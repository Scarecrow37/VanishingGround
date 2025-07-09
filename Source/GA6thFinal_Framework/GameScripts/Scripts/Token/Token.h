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
    virtual UINT8 GetTokenID() const    = 0;
    virtual UINT8 GetStackCount() const = 0;
    virtual UINT8 GetMaxStackCount() const = 0;

};

class Token : public IToken
{
public:
    Token(int tokenID);
    virtual ~Token();

public:
    void    OnRoundStart(CharacterBase* owner)  override {};
    void    OnRoundEnd(CharacterBase* owner)    override {};
    void    OnTurnStart(CharacterBase* owner)   override {};
    void    OnTurnEnd(CharacterBase* owner)     override {};
    void    OnHit(CharacterBase* owner)         override {};

    UINT8   GetTokenID() const override;
    UINT8   GetStackCount() const override;
    UINT8   GetMaxStackCount() const override;
    
public:
    void    ClearStack();
    void    SetStack(UINT8 count);
    void    AddStack(UINT8 count = 1);
    void    RemoveStack(UINT8 count = 1);
    void    SetMaxStackCount(UINT8 maxStack);

private:
    int     _tokenID    = 0;
    UINT8   _stackCount = 0;
    UINT8   _maxStackCount = UINT8_MAX;
};
