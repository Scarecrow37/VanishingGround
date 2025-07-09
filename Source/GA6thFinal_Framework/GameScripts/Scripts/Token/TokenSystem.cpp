#include "pchScripts.h"
#include "TokenSystem.h"

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
