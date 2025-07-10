#include "pchScripts.h"
#include "TokenSystem.h"
#include <TurnSystem/TurnActor/Character/CharacterBase.h>

TokenSystem::TokenSystem(CharacterBase* owner) 
    : _tokenTable(), _owner(owner)
{
    if (nullptr == _owner)
    {
        assert(false && "TokenSystem requires a valid CharacterBase owner.");
    }
}

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

void TokenSystem::NotifyCombatStart()
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnCombatStart(_owner);
        }
    }
}

void TokenSystem::NotifyRoundStart()
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnRoundStart(_owner);
        }
    }
}

void TokenSystem::NotifyRoundEnd()
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnRoundEnd(_owner);
        }
    }
}

void TokenSystem::NotifyTurnStart()
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnTurnStart(_owner);
        }
    }
}

void TokenSystem::NotifyTurnEnd()
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnTurnEnd(_owner);
        }
    }
}

void TokenSystem::NotifyHit()
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnHit(_owner);
        }
    }
}

void TokenSystem::NotifyDead()
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnDead(_owner);
        }
    }
}

void TokenSystem::NotifyKill(CharacterBase* destination)
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnKill(_owner, destination);
        }
    }
}

void TokenSystem::NotifyTokenAdded(int tokenID)
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnTokenAdded(_owner, tokenID);
        }
    }
}

void TokenSystem::NotifyTokenRemoved(int tokenID)
{
    for (auto& [tokenID, token] : _tokenTable)
    {
        if (token)
        {
            token->OnTokenRemoved(_owner, tokenID);
        }
    }
}

void TokenSystem::AddTokenStackFromID(int tokenID, UINT16 count /* = 1 */)
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
        if (_owner)
        {
            _owner->OnTokenAdded(tokenID); 
        }
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
        UINT16 curCount = token->GetStackCount();
        int    delta    = static_cast<int>(count) - static_cast<int>(curCount);
        // 음수면 스택을 줄이는 것, 양수면 스택을 늘리는 것
        if (delta < 0)
        {
            token->RemoveStack(-delta);
        }
        else if (delta > 0)
        {
            token->AddStack(delta);
        }
    }
}

void TokenSystem::RemoveTokenStackFromID(int tokenID, UINT16 count /* = 1 */)
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
        if (_owner)
        {
            _owner->OnTokenRemoved(tokenID);
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
