#include "pchScripts.h"
#include "TokenSystem.h"

void TokenSystem::OnRoundStart(CharacterBase* owner) 
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnRoundStart(owner);
        }
    }
}

void TokenSystem::OnRoundEnd(CharacterBase* owner) 
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnRoundEnd(owner);
        }
    }
}

void TokenSystem::OnTurnStart(CharacterBase* owner) 
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnTurnStart(owner);
        }
    }
}

void TokenSystem::OnTurnEnd(CharacterBase* owner) 
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnTurnEnd(owner);
        }
    }
}

void TokenSystem::OnHit(CharacterBase* owner) 
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnHit(owner);
        }
    }
}

void TokenSystem::Clear() 
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        delete token;
    }
    _tokenTable.clear();
}

IToken* TokenSystem::FindToken(int tokenID)
{
    return FindTokenEx(tokenID);
}

void TokenSystem::SetTokenStack(int tokenID, size_t count)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->SetStack(count);
    }
}

void TokenSystem::AddTokenStack(int tokenID, size_t count) 
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->AddStack(count);
    }
}

void TokenSystem::RemoveTokenStack(int tokenID, size_t count) 
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->RemoveStack(count);
    }
}

Token* TokenSystem::FindTokenEx(int tokenID)
{
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        return it->second;
    }
    return nullptr;
}
