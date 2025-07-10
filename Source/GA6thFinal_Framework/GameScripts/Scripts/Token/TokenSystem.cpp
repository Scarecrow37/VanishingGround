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

void TokenSystem::AddTokenStackFromID(int tokenID, UINT16 count)
{
    auto* token = FindTokenEx(tokenID);
    if (nullptr == token)
    {
        token = CreateTokenInstanceFromID(tokenID);
        _tokenTable[tokenID] = token;
    }
    if (token)
    {
        token->AddStack(count);
    }
}

void TokenSystem::SetTokenStackFromID(int tokenID, UINT16 count)
{
    auto* token = FindTokenEx(tokenID);
    if (nullptr == token)
    {
        token = CreateTokenInstanceFromID(tokenID);
        _tokenTable[tokenID] = token;
    }
    if (token)
    {
        token->SetStack(count);
        bool isValid = CheckValidTokenFromID(tokenID);
        if (false == isValid)
        {   // 스택이 0이 되면 토큰을 제거합니다.
            RemoveTokenFromID(tokenID);
        }
    }
}

void TokenSystem::RemoveTokenStackFromID(int tokenID, UINT16 count)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->RemoveStack(count);
        bool isValid = CheckValidTokenFromID(tokenID);
        if (false == isValid)
        {   // 스택이 0이 되면 토큰을 제거합니다.
            RemoveTokenFromID(tokenID);
        }
    }
}

IToken* TokenSystem::FindTokenFromID(int tokenID)
{
    return FindTokenEx(tokenID);
}

void TokenSystem::RemoveTokenFromID(int tokenID)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        _tokenTable.erase(tokenID);
        delete token;
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

Token* TokenSystem::FindTokenEx(std::string_view tokenName)
{
    int id = GetTokenIDFromName(tokenName);
    return FindTokenEx(id);
}

bool TokenSystem::CheckValidTokenFromID(int tokenID)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        UINT16 stackCount = token->GetStackCount();
        return 0 < stackCount;
    }
    return false;
}

Token* TokenSystem::CreateTokenInstanceFromID(int tokenID)
{
    auto it = _tokenIDFactoryTable.find(tokenID);
    if (it != _tokenIDFactoryTable.end())
    {
        auto* instance = it->second();
        return instance;
    }
    return nullptr;
}

Token* TokenSystem::CreateTokenInstanceFromName(std::string_view tokenName)
{
    auto it = _tokenNameFactoryTable.find(tokenName.data());
    if (it != _tokenNameFactoryTable.end())
    {
        auto* instance = it->second();
        return instance;
    }
    return nullptr;
}

int TokenSystem::GetTokenIDFromName(std::string_view tokenName) const
{
    auto it = _tokenNameToIDTable.find(tokenName.data());
    if (it != _tokenNameToIDTable.end())
    {
        return it->second;
    }
    return 0;
}

const std::string& TokenSystem::GetTokenNameFromID(int tokenID) const
{
    auto it = _tokenIDToNameTable.find(tokenID);
    if (it != _tokenIDToNameTable.end())
    {
        return it->second;
    }
    static const std::string emptyString;
    return emptyString; // 토큰이 존재하지 않으면 빈 문자열 반환
}
