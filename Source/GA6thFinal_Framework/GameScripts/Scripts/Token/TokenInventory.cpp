#include "pchScripts.h"
#include "TokenInventory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"

TokenInventory::TokenInventory(CharacterBase* owner) 
    : _tokenTable(), _owner(*owner)
{
    // 토큰 테이블 초기화
    InitTokenInstance();
}

TokenInventory::~TokenInventory() 
{
    _tokenTable.clear();
    _vaildTokenVector.clear();
}

void TokenInventory::Clear()
{
    for (auto& [id, token] : _tokenTable)
    {
        RemoveTokenFromID(id);
    }
}

void TokenInventory::NotifyCombatStart()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnCombatStart(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyRoundStart()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnRoundStart(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyRoundEnd()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnRoundEnd(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyEachTurnStart(CharacterBase* destination) 
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnEachTurnStart(&_owner, destination);
            }
        }
    }
}

void TokenInventory::NotifyTurnStart()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnTurnStart(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyTurnEnd()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnTurnEnd(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyHit()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnHit(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyDead()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnDead(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyKill(CharacterBase* destination)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnKill(&_owner, destination);
            }
        }
    }
}

void TokenInventory::NotifyTokenAdded(int tokenID)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnTokenAdded(&_owner, tokenID);
            }
        }
    }
}

void TokenInventory::NotifyTokenRemoved(int tokenID)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnTokenRemoved(&_owner, tokenID);
            }
        }
    }
}

void TokenInventory::NotifyQTEStart()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnQTEStart(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyQTEEnd()
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& instance : instances)
    {
        Token* token = instance;
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnQTEEnd(&_owner);
            }
        }
    }
}

void TokenInventory::NotifyPreBattleCalculateChain(Player& source, PlayerStats& sourceStats, WeaponStats& weaponStats,
                                                   Enemy& dest, EnemyStats& destStats)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& token : instances)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnPreBattleCalculateChain(source, sourceStats, weaponStats, dest, destStats);
            }
        }
    }
}

void TokenInventory::NotifyPreBattleCalculateChain(Enemy& source, EnemyStats& sourceStats, Player& dest,
                                                   PlayerStats& destStats)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& token : instances)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnPreBattleCalculateChain(source, sourceStats, dest, destStats);
            }
        }
    }
}

void TokenInventory::NotifyPreAttackBattleCalculateDamage(Player& source, PlayerStats& sourceStats,
                                                          WeaponStats& weaponStats, Enemy& dest, EnemyStats& destStats)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& token : instances)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnPreAttackBattleCalculateDamage(source, sourceStats, weaponStats, dest, destStats);
            }
        }
    }
}

void TokenInventory::NotifyPreAttackBattleCalculateDamage(Enemy& source, EnemyStats& sourceStats, Player& dest,
                                                          PlayerStats& destStats)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& token : instances)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnPreAttackBattleCalculateDamage(source, sourceStats, dest, destStats);
            }
        }
    }
}

void TokenInventory::NotifyPreHitBattleCalculateDamage(Player& source, PlayerStats& sourceStats, Enemy& dest, EnemyStats& destStats)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& token : instances)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnPreHitBattleCalculateDamage(source, sourceStats, dest, destStats);
            }
        }
    }
}

void TokenInventory::NotifyPreHitBattleCalculateDamage(Enemy& source, EnemyStats& sourceStats, Player& dest,
                                                       PlayerStats& destStats)
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& token : instances)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnPreHitBattleCalculateDamage(source, sourceStats, dest, destStats);
            }
        }
    }
}

void TokenInventory::NotifyTakeDamage(int& damage) 
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& token : instances)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnTakeDamage(&_owner, damage);
            }
        }
    }
}

void TokenInventory::NotifyRollRandomSpeed(int& randomSpeed) 
{
    const auto& instances = TokenSystem::GetTokenInstances();
    for (auto& token : instances)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                token->OnRollRandomSpeed(&_owner, randomSpeed);
            }
        }
    }
}

void TokenInventory::AddTokenStackFromID(int tokenID, int count /* = 1 */)
{
    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        turnMode->ApplyActions([&](TurnAction& action) 
        { 
            action.OnTokenAddedStart(_owner, tokenID, count);
        });
    }

    if (0 == count)
    {   // 추가할 스택이 0이면 아무것도 하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
        return;
    }

    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        int maxStackCount = UINT_MAX;
        IToken* token = TokenSystem::GetTokenFromID(tokenID);
        if (token && token->CanAdd(&_owner))
        {
            maxStackCount   = token->GetMaxStackCount();
            int& stackCount = it->second;
            stackCount += count;
            stackCount = std::min(maxStackCount, stackCount);
            UpdateToken(tokenID);
            _owner.OnTokenAdded(tokenID);
        }
    }

    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        turnMode->ApplyActions([&](TurnAction& action) 
        { 
            action.OnTokenAddedEnd(_owner, tokenID, count); 
        });
    }
}

void TokenInventory::SetTokenStackFromID(int tokenID, int count)
{
    int curCount = GetTokenStackFromID(tokenID);
    if (curCount == count)
    { // 현재 스택과 설정하려는 스택이 같으면 아무것도 하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
        return;
    }
    int delta = count - curCount;
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

void TokenInventory::RemoveTokenStackFromID(int tokenID, int count /* = 1 */)
{
    if (0 == count)
    {   // 제거할 스택이 0이면 아무것도 하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
        return;
    }
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        IToken* token = TokenSystem::GetTokenFromID(tokenID);
        if (token && token->CanRemove(&_owner))
        {
            int& stackCount = it->second;
            stackCount -= count;
            stackCount = std::max(0, stackCount);
            UpdateToken(tokenID);
            _owner.OnTokenRemoved(tokenID);
        }
    }
}

void TokenInventory::RemoveTokenFromID(int tokenID)
{
    auto iter =_tokenTable.find(tokenID);
    if (iter != _tokenTable.end())
    {
        int* count = &iter->second;
        if (0 < *count)
        {
            *count = 0; // 스택 카운트를 0으로 설정하여 토큰을 제거합니다.
            UpdateToken(tokenID);
        }
    }
}

bool TokenInventory::HasTokenFromID(int tokenID) const
{
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        bool isValid = 0 < it->second;
        return isValid;
    }
    return false;
}

bool TokenInventory::HasTokenFromTag(TokenTag tag) const
{
    const auto& tagTokens = TokenSystem::GetTokenInstancesFromTag(tag);
    for (const auto& token : tagTokens)
    {
        if (token)
        {
            int count = GetTokenStackFromID(token->GetTokenID());
            if (0 < count)
            {
                return true; // 해당 태그에 유효한 토큰이 존재합니다.
            }
        }
    }
    return false;
}

int TokenInventory::GetTokenStackFromID(int tokenID) const
{
    auto it = _tokenTable.find(tokenID);
    if (it != _tokenTable.end())
    {
        return it->second;
    }
    return 0;
}

int TokenInventory::GetTokenStackFromTag(TokenTag tokenTag) const
{
    int         total     = 0;
    const auto& tagTokens = TokenSystem::GetTokenInstancesFromTag(tokenTag);
    for (const auto& token : tagTokens)
    {
        if (token)
        {
            total += GetTokenStackFromID(token->GetTokenID());
        }
    }
    return total;
}

size_t TokenInventory::GetValidTokenCount() const
{
    return _vaildTokenVector.size();
}

bool TokenInventory::IsEmpty() const
{
    return _vaildTokenVector.empty();
}

void TokenInventory::DrawImGuiDebugData() 
{
    // ValidTokenStack
    ImGui::BeginChild("ValidTokenStack", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);
    const auto& instances = TokenSystem::GetTokenInstances();
    for (size_t i = 0; i < _vaildTokenVector.size(); ++i)
    {
        TokenID tokenID = _vaildTokenVector[i];
        const auto& tokenName = TokenSystem::GetTokenNameFromID(tokenID);
        int tokenCount = GetTokenStackFromID(tokenID);
        std::string tokenInfo = std::format("{} ({})", tokenName, tokenCount);
        if (ImGui::Selectable(tokenInfo.c_str(), false))
        {
            // 선택된 토큰에 대한 추가 작업이 필요하면 여기에 작성
        }
        if (i < _vaildTokenVector.size() - 1)
        {
            ImGui::Separator();
        }
    }
    ImGui::EndChild();
    
    if (ImGui::TreeNodeEx("Token Debug##token inventory"))
    {
        if (ImGui::TreeNodeEx("Token Instances##token inventory"))
        {
            ImGui::Text("Token Count: %zu", _vaildTokenVector.size());
            ImGui::Text("Total Tokens: %zu", instances.size());
            ImGui::BeginChild("TokenList", ImVec2(0, 200), true);
            for (const auto& token : instances)
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

        static int id = 0;
        const std::string& tokenName = SingletonComponent<TokenSystem>::GetInstance()->GetTokenNameFromID(id);
        if (ImGui::TreeNodeEx("Add or Remove Token##token inventory"))
        {
            if (ImGui::BeginCombo("##Add or Remove TokenStack", tokenName.c_str()))
            {
                const auto& instances = TokenSystem::GetTokenInstances();
                for (const auto& token : instances)
                {
                    if (token)
                    {
                        if (ImGui::Selectable(token->GetTokenName(), false))
                        {
                            id = token->GetTokenID();
                        }
                    }
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Add"))
            {
                AddTokenStackFromID(id, 1); // 스택을 1개 추가합니다.
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove"))
            {
                RemoveTokenStackFromID(id, 1); // 스택을 1개 제거합니다.
            }
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

void TokenInventory::InitTokenInstance()
{
    // 테이블에 존재하는 토큰을 모두 초기화합니다.
    _tokenTable.clear();
    auto& tokenVector = TokenSystem::GetTokenInstances();
    for (const auto& token : tokenVector)
    {
        if (token)
        {
            _tokenTable[token->GetTokenID()] = 0;
        }
    }
}

void TokenInventory::UpdateToken(TokenID tokenID)
{
    auto iter = _tokenTable.find(tokenID);
    if (iter != _tokenTable.end())
    {
        int count = iter->second;
        if (0 < count)
        {
            // 유효한 토큰 벡터에 추가
            auto it = std::find(_vaildTokenVector.begin(), _vaildTokenVector.end(), tokenID);
            if (it == _vaildTokenVector.end())
            {
                _vaildTokenVector.push_back(tokenID);
            }
        }
        else
        {
            // 유효한 토큰 벡터에서 제거
            auto it = std::find(_vaildTokenVector.begin(), _vaildTokenVector.end(), tokenID);
            if (it != _vaildTokenVector.end())
            {
                _vaildTokenVector.erase(it);
            }
        }
    }
}

bool TokenInventory::CheckValidTokenFromID(TokenID tokenID)
{
    auto iter = _tokenTable.find(tokenID);
    if (iter != _tokenTable.end())
    {
        auto it = std::find(_vaildTokenVector.begin(), _vaildTokenVector.end(), tokenID);
        if (it != _vaildTokenVector.end())
        {
            return true;
        }
    }
    return false;
}
