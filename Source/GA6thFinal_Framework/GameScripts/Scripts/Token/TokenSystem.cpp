#include "pchScripts.h"
#include "TokenSystem.h"

void TokenSystem::Clear()
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            delete token;
        }
    }
    _tokenTable.clear();
}

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

void TokenSystem::AddTokenStackFromID(int tokenID, UINT8 count)
{
    auto* token = FindTokenEx(tokenID);
    if (nullptr == token)
    {
        token = CreateTokenInstanceFromID(tokenID);
    }
    if (token)
    {
        token->AddStack(count);
    }
}

void TokenSystem::SetTokenStack(int tokenID, UINT8 count)
{
    auto* token = FindTokenEx(tokenID);
    if (nullptr == token)
    {
        token = CreateTokenInstanceFromID(tokenID);
    }
    if (token)
    {
        token->SetStack(count);
    }
}

void TokenSystem::RemoveTokenStack(int tokenID, UINT8 count)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->RemoveStack(count);
    }
}

IToken* TokenSystem::FindToken(int tokenID)
{
    return FindTokenEx(tokenID);
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

void TokenSystem::CheckValidToken(int tokenID) 
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        UINT8 stackCount = token->GetStackCount();
        if (0 == stackCount)
        {
            _tokenTable.erase(tokenID);
            delete token;
        }
    }
}

Token* TokenSystem::CreateTokenInstanceFromID(int tokenID)
{
    auto it = _tokenIDFactoryTable.find(tokenID);
    if (it != _tokenIDFactoryTable.end())
    {
        return it->second();
    }
    return nullptr;
}

Token* TokenSystem::CreateTokenInstanceFromName(std::string_view tokenName)
{
    auto it = _tokenNameFactoryTable.find(tokenName.data());
    if (it != _tokenNameFactoryTable.end())
    {
        return it->second();
    }
    return nullptr;
}
