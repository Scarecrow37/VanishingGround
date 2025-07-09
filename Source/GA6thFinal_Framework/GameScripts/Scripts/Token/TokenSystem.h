#pragma once
#include "Token.h"

class TokenSystem
{
public:
    TokenSystem()  = default;
    ~TokenSystem() = default;

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
