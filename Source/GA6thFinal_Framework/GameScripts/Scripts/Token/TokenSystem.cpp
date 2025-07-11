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
    // 토큰 테이블 초기화
    InitTokenInstance();
}

void TokenSystem::Clear()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->SetStack(0); // 스택을 0으로 설정하여 토큰을 초기화합니다.
        }
    }
}

void TokenSystem::NotifyCombatStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnCombatStart(_owner);
        }
    }
}

void TokenSystem::NotifyRoundStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnRoundStart(_owner);
        }
    }
}

void TokenSystem::NotifyRoundEnd()
{
    for (auto& token : _tokenInstances)
    {
        if (token)
        {
            token->OnRoundEnd(_owner);
        }
    }
}

void TokenSystem::NotifyTurnStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTurnStart(_owner);
        }
    }
}

void TokenSystem::NotifyTurnEnd()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTurnEnd(_owner);
        }
    }
}

void TokenSystem::NotifyHit()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnHit(_owner);
        }
    }
}

void TokenSystem::NotifyDead()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnDead(_owner);
        }
    }
}

void TokenSystem::NotifyKill(CharacterBase* destination)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnKill(_owner, destination);
        }
    }
}

void TokenSystem::NotifyTokenAdded(int tokenID)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTokenAdded(_owner, tokenID);
        }
    }
}

void TokenSystem::NotifyTokenRemoved(int tokenID)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTokenRemoved(_owner, tokenID);
        }
    }
}

void TokenSystem::AddTokenStackFromID(int tokenID, UINT16 count /* = 1 */)
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

void TokenSystem::SetTokenStackFromID(int tokenID, UINT16 count)
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

void TokenSystem::RemoveTokenStackFromID(int tokenID, UINT16 count /* = 1 */)
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

IToken* TokenSystem::FindTokenFromID(int tokenID)
{
    return FindTokenEx(tokenID);
}

void TokenSystem::RemoveTokenFromID(int tokenID)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->SetStack(0); // 스택을 0으로 설정하여 토큰을 제거합니다.
    }
}

bool TokenSystem::HasToken(int tokenID) const
{
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        bool isValid = 0 < it->second->GetStackCount();
        return isValid;                                
    }
    return false;
}

bool TokenSystem::IsEmpty() const
{
    return _validTokenTable.empty();
}

void TokenSystem::InitTokenInstance()
{
    // 테이블에 존재하는 토큰을 모두 초기화합니다.
    _tokenTable.clear();
    for (const auto& [tokenID, factory] : _tokenIDFactoryTable)
    {
        auto* token = factory();
        if (token)
        {
            _tokenTable[tokenID] = token;
            token->SetDirtyCountCallback([this](int id) { UpdateToken(id); });
            token->SetDirtyOrderCallback([this](int id) { SortByOrder(); });
        }
    }
}

void TokenSystem::SortByOrder() 
{   // 토큰을 Order에 따라 내림차순으로 정렬합니다.
    std::sort(_tokenInstances.begin(), _tokenInstances.end(),
              [](Token* a, Token* b) { return a->GetTokenOrder() > b->GetTokenOrder(); 
        });
}

void TokenSystem::UpdateToken(int tokenID)
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

Token* TokenSystem::FindTokenEx(int tokenID)
{
    Token* result = nullptr;
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        result = it->second;
    }
    else
    {
        result = CreateTokenInstanceFromID(tokenID);
        _tokenTable[tokenID] = result;
    }
    return result;
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
