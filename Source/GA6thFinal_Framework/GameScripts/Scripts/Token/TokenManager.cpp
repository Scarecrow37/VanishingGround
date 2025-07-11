#include "pchScripts.h"
#include "TokenManager.h"
#include <TurnSystem/TurnActor/Character/CharacterBase.h>

TokenManager::TokenManager(CharacterBase* owner) 
    : _tokenTable(), _owner(owner)
{
    if (nullptr == _owner)
    {
        assert(false && "TokenManager requires a valid CharacterBase owner.");
    }
    // 토큰 테이블 초기화
    InitTokenInstance();
}

void TokenManager::Clear()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->SetStack(0); // 스택을 0으로 설정하여 토큰을 초기화합니다.
        }
    }
}

void TokenManager::NotifyCombatStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnCombatStart(_owner);
        }
    }
}

void TokenManager::NotifyRoundStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnRoundStart(_owner);
        }
    }
}

void TokenManager::NotifyRoundEnd()
{
    for (auto& token : _tokenInstances)
    {
        if (token)
        {
            token->OnRoundEnd(_owner);
        }
    }
}

void TokenManager::NotifyTurnStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTurnStart(_owner);
        }
    }
}

void TokenManager::NotifyTurnEnd()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTurnEnd(_owner);
        }
    }
}

void TokenManager::NotifyHit()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnHit(_owner);
        }
    }
}

void TokenManager::NotifyDead()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnDead(_owner);
        }
    }
}

void TokenManager::NotifyKill(CharacterBase* destination)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnKill(_owner, destination);
        }
    }
}

void TokenManager::NotifyTokenAdded(int tokenID)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTokenAdded(_owner, tokenID);
        }
    }
}

void TokenManager::NotifyTokenRemoved(int tokenID)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTokenRemoved(_owner, tokenID);
        }
    }
}

void TokenManager::AddTokenStackFromID(int tokenID, UINT16 count /* = 1 */)
{
    if (0 == count)
    {   // 추가할 스택이 0이면 아무것도 하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
        return;
    }
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->AddStack(count);
        if (_owner)
        {
            _owner->OnTokenAdded(tokenID); 
        }
    }
}

void TokenManager::SetTokenStackFromID(int tokenID, UINT16 count)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        UINT16 curCount = token->GetStackCount();
        if (curCount == count)
        {   // 현재 스택과 설정하려는 스택이 같으면 아무것도 하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
            return;
        }
        int delta = static_cast<int>(count) - static_cast<int>(curCount);
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

void TokenManager::RemoveTokenStackFromID(int tokenID, UINT16 count /* = 1 */)
{
    if (0 == count)
    {   // 제거할 스택이 0이면 아무것도 하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
        return;
    }
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->RemoveStack(count);
        bool isValid = CheckValidTokenFromID(tokenID);
        if (_owner)
        {
            _owner->OnTokenRemoved(tokenID);
        }
    }
}

IToken* TokenManager::FindTokenFromID(int tokenID)
{
    return FindTokenEx(tokenID);
}

void TokenManager::RemoveTokenFromID(int tokenID)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->SetStack(0); // 스택을 0으로 설정하여 토큰을 제거합니다.
    }
}

bool TokenManager::HasToken(int tokenID) const
{
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        bool isValid = 0 < it->second->GetStackCount();
        return isValid;                                
    }
    return false;
}

bool TokenManager::IsEmpty() const
{
    return _validTokenTable.empty();
}

void TokenManager::InitTokenInstance()
{
    // 테이블에 존재하는 토큰을 모두 초기화합니다.
    _tokenTable.clear();
    auto& table = TokenSystem::GetTokenIDToNameTable();
    for (const auto& [tokenID, tokenName] : table)
    {
        Token* token = nullptr;
        if (TokenSystem::CreateTokenInstanceFromID(tokenID, &token) && token)
        {
            _tokenTable[tokenID] = token;
            _tokenInstances.push_back(token); // 토큰 인스턴스 리스트에 추가
            token->SetDirtyCountCallback([this](int id) { UpdateToken(id); });
            token->SetDirtyOrderCallback([this](int id) { SortByOrder(); });
        }
    }
    SortByOrder(); // 초기화 후 토큰을 정렬합니다.
}

void TokenManager::SortByOrder() 
{   // 토큰을 Order에 따라 내림차순으로 정렬합니다.
    std::sort(_tokenInstances.begin(), _tokenInstances.end(),
              [](Token* a, Token* b) { return a->GetTokenOrder() > b->GetTokenOrder(); 
        });
}

void TokenManager::UpdateToken(int tokenID)
{
    auto token = FindTokenEx(tokenID);
    if (token)
    {
        UINT16 count = token->GetStackCount();
        if (0 < count)
        {
            _validTokenTable[token->GetTokenID()] = token;  // 유효한 토큰 테이블에 추가
        }
        else
        {
            _validTokenTable.erase(token->GetTokenID());    // 유효한 토큰 테이블에서 제거
        }
    }
}

Token* TokenManager::FindTokenEx(int tokenID)
{
    Token* result = nullptr;
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        result = it->second;
    }
    else
    {
        auto system = TokenSystem::GetInstance();
        if (system)
        {
            if (system->CreateTokenInstanceFromID(tokenID, &result))
            {
                _tokenTable[tokenID] = result;
            }
        }
    }
    return result;
}

Token* TokenManager::FindTokenEx(std::string_view tokenName)
{
    auto system = TokenSystem::GetInstance();
    if (system)
    {
        int id = system->GetTokenIDFromName(tokenName);
        return FindTokenEx(id);
    }
    return nullptr;
}

bool TokenManager::CheckValidTokenFromID(int tokenID)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        UINT16 stackCount = token->GetStackCount();
        return 0 < stackCount;
    }
    return false;
}
