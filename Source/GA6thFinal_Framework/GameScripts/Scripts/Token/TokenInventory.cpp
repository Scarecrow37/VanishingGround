#include "pchScripts.h"
#include "TokenInventory.h"
#include <TurnSystem/TurnActor/Character/CharacterBase.h>

TokenInventory::TokenInventory(CharacterBase* owner) 
    : _tokenTable(), _owner(owner)
{
    if (nullptr == _owner)
    {
        assert(false && "TokenInventory requires a valid CharacterBase owner.");
    }
    // 토큰 테이블 초기화
    InitTokenInstance();
}

TokenInventory::~TokenInventory() 
{
    for (auto& [id, token] : _tokenTable)
    {
        if (token)
        {
            delete token; // 토큰 인스턴스 메모리 해제
            token = nullptr;
        }
    }
    _tokenTable.clear();
    _vaildTokenVector.clear();
    _tokenInstances.clear();
    _owner = nullptr; // 소유자 초기화
}

void TokenInventory::Clear()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->SetStack(0); // 스택을 0으로 설정하여 토큰을 초기화합니다.
        }
    }
}

void TokenInventory::NotifyCombatStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnCombatStart(_owner);
        }
    }
}

void TokenInventory::NotifyRoundStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnRoundStart(_owner);
        }
    }
}

void TokenInventory::NotifyRoundEnd()
{
    for (auto& token : _tokenInstances)
    {
        if (token)
        {
            token->OnRoundEnd(_owner);
        }
    }
}

void TokenInventory::NotifyTurnStart()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTurnStart(_owner);
        }
    }
}

void TokenInventory::NotifyTurnEnd()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTurnEnd(_owner);
        }
    }
}

void TokenInventory::NotifyHit()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnHit(_owner);
        }
    }
}

void TokenInventory::NotifyDead()
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnDead(_owner);
        }
    }
}

void TokenInventory::NotifyKill(CharacterBase* destination)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnKill(_owner, destination);
        }
    }
}

void TokenInventory::NotifyTokenAdded(int tokenID)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTokenAdded(_owner, tokenID);
        }
    }
}

void TokenInventory::NotifyTokenRemoved(int tokenID)
{
    for (auto& token : _tokenInstances)
    {
        if (token && 0 < token->GetStackCount())
        {
            token->OnTokenRemoved(_owner, tokenID);
        }
    }
}

void TokenInventory::AddTokenStackFromID(int tokenID, UINT16 count /* = 1 */)
{
    if (0 == count)
    {   // 추가할 스택이 0이면 아무것도 하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
        return;
    }
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        if (0 == token->GetStackCount())
        {   // 스택이 0인 토큰을 유효한 토큰 벡터에 추가
            _vaildTokenVector.push_back(token); 
        }
        token->AddStack(count);
        if (_owner)
        {
            _owner->OnTokenAdded(tokenID); 
        }
    }
}

void TokenInventory::SetTokenStackFromID(int tokenID, UINT16 count)
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
            RemoveTokenStackFromID(tokenID, static_cast<UINT16>(std::abs(delta))); // 음수면 제거
        }
        else if (delta > 0)
        {
            AddTokenStackFromID(tokenID, static_cast<UINT16>(delta)); // 양수면 추가
        }
    }
}

void TokenInventory::RemoveTokenStackFromID(int tokenID, UINT16 count /* = 1 */)
{
    if (0 == count)
    {   // 제거할 스택이 0이면 아무것도 하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
        return;
    }
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        token->RemoveStack(count);
        if (_owner)
        {
            _owner->OnTokenRemoved(tokenID);
        }
    }
}

IToken* TokenInventory::FindTokenFromID(int tokenID)
{
    return FindTokenEx(tokenID);
}

IToken* TokenInventory::FindVaildTokenFromID(int tokenID)
{
    Token* token = FindTokenEx(tokenID);
    if (token)
    {
        auto it = std::find(_vaildTokenVector.begin(), _vaildTokenVector.end(), token);
        if (it != _vaildTokenVector.end())
        {
            return *it; // 유효한 토큰 벡터에서 찾은 토큰을 반환합니다.
        }
    }
    return nullptr;
}

void TokenInventory::RemoveTokenFromID(int tokenID)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        // 스택을 0으로 설정하여 토큰을 제거합니다.
        token->SetStack(0);

        // 유효한 토큰 벡터에서 제거
        auto it = std::find(_vaildTokenVector.begin(), _vaildTokenVector.end(), token);
        if (it != _vaildTokenVector.end())
        {
            _vaildTokenVector.erase(it);
        }
    }
}

bool TokenInventory::HasToken(int tokenID) const
{
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        bool isValid = 0 < it->second->GetStackCount();
        return isValid;
    }
    return false;
}

size_t TokenInventory::GetTokenCount() const
{
    return _tokenInstances.size();
}

bool TokenInventory::IsEmpty() const
{
    return _vaildTokenVector.empty();
}

void TokenInventory::DrawImGuiDebugData() 
{
    // ValidTokenStack
    ImGui::BeginChild("ValidTokenStack");
    for (size_t i = 0; i < _vaildTokenVector.size(); ++i)
    {
        auto& token = _vaildTokenVector[i];
        if (token)
        {
            if (ImGui::Selectable(token->GetTokenName(), false))
            {
                // 선택된 토큰에 대한 추가 작업이 필요하면 여기에 작성
            }
            ImGui::Separator();
        }
    }
    ImGui::EndChild();

    if (ImGui::TreeNodeEx("TokenInstances"))
    {
        ImGui::Text("Token Count: %zu", _vaildTokenVector.size());
        ImGui::Text("Total Tokens: %zu", _tokenInstances.size());
        ImGui::BeginChild("TokenList", ImVec2(0, 200), true);
        for (const auto& token : _tokenInstances)
        {
            if (token)
            {
                if (ImGui::Selectable(token->GetTokenName(), false))
                {
                }
            }
        }
        ImGui::EndChild();
        ImGui::TreePop();
    }
}

void TokenInventory::InitTokenInstance()
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
            token->SetDirtyCountCallback([this](int id) { UpdateToken(id);  });
            token->SetDirtyOrderCallback([this](int id) { SortByOrder();    });
        }
    }
    SortByOrder(); // 초기화 후 토큰을 정렬합니다.
}

void TokenInventory::SortByOrder() 
{   // 토큰을 Order에 따라 오름차순 정렬합니다.
    std::sort(_tokenInstances.begin(), _tokenInstances.end(),
              [](Token * a, Token* b) { return a->GetTokenOrder() < b->GetTokenOrder(); 
        });
}

void TokenInventory::UpdateToken(int tokenID)
{
    auto token = FindTokenEx(tokenID);
    if (token)
    {
        UINT16 count = token->GetStackCount();
        if (0 < count)
        {
            // 유효한 토큰 벡터에 추가
            auto it = std::find(_vaildTokenVector.begin(), _vaildTokenVector.end(), token);
            if (it == _vaildTokenVector.end())
            {
                _vaildTokenVector.push_back(token);
            }
        }
        else
        {
            // 유효한 토큰 벡터에서 제거
            auto it = std::find(_vaildTokenVector.begin(), _vaildTokenVector.end(), token);
            if (it != _vaildTokenVector.end())
            {
                _vaildTokenVector.erase(it);
            }
        }
    }
}

Token* TokenInventory::FindTokenEx(int tokenID)
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

Token* TokenInventory::FindTokenEx(std::string_view tokenName)
{
    auto system = TokenSystem::GetInstance();
    if (system)
    {
        int id = system->GetTokenIDFromName(tokenName);
        return FindTokenEx(id);
    }
    return nullptr;
}

bool TokenInventory::CheckValidTokenFromID(int tokenID)
{
    auto* token = FindTokenEx(tokenID);
    if (token)
    {
        return nullptr != FindVaildTokenFromID(tokenID);
    }
    return false;
}
