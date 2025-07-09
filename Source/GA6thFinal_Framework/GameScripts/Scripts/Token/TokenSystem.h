#pragma once
#include "Token.h"

class TokenSystem
{
public:
    TokenSystem()  = default;
    ~TokenSystem() = default;

public:
    void OnRoundStart(CharacterBase* owner);
    void OnRoundEnd(CharacterBase* owner);
    void OnTurnStart(CharacterBase* owner);  
    void OnTurnEnd(CharacterBase* owner); 
    void OnHit(CharacterBase* owner);

    void Clear();

public:
    IToken* FindToken(int tokenID);
    void    SetTokenStack(int tokenID, size_t count);
    void    AddTokenStack(int tokenID, size_t count);
    void    RemoveTokenStack(int tokenID, size_t count);

private:
    Token*  FindTokenEx(int tokenID);

private:
    std::unordered_map<int, Token*> _tokenTable;
};
