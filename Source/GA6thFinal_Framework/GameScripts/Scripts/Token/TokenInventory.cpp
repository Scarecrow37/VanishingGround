#include "pchScripts.h"
#include "TokenInventory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "Token/TokenSystem.h"

namespace
{
    TokenSystem* GetTokenSystem()
    {
        return SingletonComponent<TokenSystem>::GetInstance();
    }
    IToken* GetTokenFromID(int tokenID)
    {
        if (TokenSystem* tokenSystem = GetTokenSystem())
        {
            return tokenSystem->GetTokenFromID(tokenID);
        }
        return nullptr;
    }
    void NotifyTokenEvent(std::function<void(Token&)> func)
    {
        if (TokenSystem* tokenSystem = GetTokenSystem())
        {
            const auto& instances = tokenSystem->GetTokenInstances();
            for (auto& token : instances)
            {
                if (token && func)
                {
                    func(*token);
                }
            }
        }
    }
    const char* GetTokenNameFromID(int tokenID)
    {
        if (TokenSystem* tokenSystem = GetTokenSystem())
        {
            if (const TokenData* tokenData = tokenSystem->GetTokenDataFromID(tokenID))
            {
                return tokenData->Name.c_str();
            }
        }
        return "";
    }
}

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
    NotifyTokenEvent([this](Token& token)
    {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnCombatStart(&_owner);
        }
    });
}

void TokenInventory::NotifyRoundStart()
{
    NotifyTokenEvent([this](Token& token)
    {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnRoundStart(&_owner);
        }
    });
}

void TokenInventory::NotifyRoundEnd()
{
    NotifyTokenEvent([this](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnRoundEnd(&_owner);
        }
    });
}

void TokenInventory::NotifyEachTurnStart(CharacterBase* destination) 
{
    NotifyTokenEvent([this, destination](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnEachTurnStart(&_owner, destination);
        }
    });
}

void TokenInventory::NotifyTurnStart()
{
    NotifyTokenEvent([this](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnTurnStart(&_owner);
        }
    });
}

void TokenInventory::NotifyTurnEnd()
{
    NotifyTokenEvent([this](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnTurnEnd(&_owner);
        }
    });
}

void TokenInventory::NotifyHit()
{
    NotifyTokenEvent([this](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnHit(&_owner);
        }
    });
}

void TokenInventory::NotifyDead()
{
    NotifyTokenEvent([this](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnDead(&_owner);
        }
    });
}

void TokenInventory::NotifyKill(CharacterBase* destination)
{
    NotifyTokenEvent([this, destination](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnKill(&_owner, destination);
        }
    });
}

void TokenInventory::NotifyTokenAdded(int tokenID)
{
    NotifyTokenEvent([this, tokenID](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnTokenAdded(&_owner, tokenID);
        }
    });
}

void TokenInventory::NotifyTokenRemoved(int tokenID)
{
    NotifyTokenEvent([this, tokenID](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnTokenRemoved(&_owner, tokenID);
        }
    });
}

void TokenInventory::NotifyQTEStart()
{
    NotifyTokenEvent([this](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnQTEStart(&_owner);
        }
    });
}

void TokenInventory::NotifyQTEEnd()
{
    NotifyTokenEvent([this](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnQTEEnd(&_owner);
        }
    });
}

void TokenInventory::NotifyPreBattleCalculateChain(Player& source, PlayerStats& sourceStats, WeaponStats& weaponStats,
                                                   Enemy& dest, EnemyStats& destStats)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreBattleCalculateChain(source, sourceStats, weaponStats, dest, destStats);
        }
    });
}

void TokenInventory::NotifyPreBattleCalculateChain(Enemy& source, EnemyStats& sourceStats, Player& dest,
                                                   PlayerStats& destStats)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreBattleCalculateChain(source, sourceStats, dest, destStats);
        }
    });
}

void TokenInventory::NotifyPreAttackBattleCalculateDamage(Player& source, PlayerStats& sourceStats,
                                                          WeaponStats& weaponStats, Enemy& dest, EnemyStats& destStats)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreAttackBattleCalculateDamage(source, sourceStats, weaponStats, dest, destStats);
        }
    });
}

void TokenInventory::NotifyPreAttackBattleCalculateDamage(Enemy& source, EnemyStats& sourceStats, Player& dest,
                                                          PlayerStats& destStats)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreAttackBattleCalculateDamage(source, sourceStats, dest, destStats);
        }
    });
}

void TokenInventory::NotifyPreHitBattleCalculateDamage(Player& source, PlayerStats& sourceStats, Enemy& dest, EnemyStats& destStats)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreHitBattleCalculateDamage(source, sourceStats, dest, destStats);
        }
    });
}

void TokenInventory::NotifyPreHitBattleCalculateDamage(Enemy& source, EnemyStats& sourceStats, Player& dest,
                                                       PlayerStats& destStats)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreHitBattleCalculateDamage(source, sourceStats, dest, destStats);
        }
    });
}

void TokenInventory::NotifyTakeDamage(int& damage) 
{
    NotifyTokenEvent([this, &damage](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnTakeDamage(&_owner, damage);
        }
    });
}

void TokenInventory::NotifyRollRandomSpeed(int& randomSpeed) 
{
    NotifyTokenEvent([this, &randomSpeed](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnRollRandomSpeed(&_owner, randomSpeed);
        }
    });
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
        IToken* token = GetTokenFromID(tokenID);
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
        IToken* token = GetTokenFromID(tokenID);
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

bool TokenInventory::HasTokenFromTag(const std::string& tag) const
{
    if (TokenSystem* tokenSystem = GetTokenSystem())
    {
        if (auto* set = tokenSystem->GetTokenInstancesFromTag(tag))
        {
            for (auto& token : *set)
            {
                bool valid = HasTokenFromID(token->GetTokenID());
                return valid;
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
    if (TokenSystem* tokenSystem = GetTokenSystem())
    {
        const auto& instances = tokenSystem->GetTokenInstances();

        ImGui::BeginChild("ValidTokenStack", ImVec2(0, 0), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);
        for (size_t i = 0; i < _vaildTokenVector.size(); ++i)
        {
            TokenID     tokenID     = _vaildTokenVector[i];
            const auto& tokenName   = GetTokenNameFromID(tokenID);
            int         tokenCount  = GetTokenStackFromID(tokenID);
            std::string tokenInfo   = std::format("{} ({})", tokenName, tokenCount);
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
                        const std::string& name = token->GetTokenName();
                        if (ImGui::Selectable(name.c_str(), false))
                        {
                        }
                    }
                }
                ImGui::EndChild();
                ImGui::TreePop();
            }

            static int  id        = 0;
            const char* tokenName = GetTokenNameFromID(id);
            if (ImGui::TreeNodeEx("Add or Remove Token##token inventory"))
            {
                if (ImGui::BeginCombo("##Add or Remove TokenStack", tokenName))
                {
                    for (const auto& token : instances)
                    {
                        if (token)
                        {
                            const std::string& name = token->GetTokenName();
                            if (ImGui::Selectable(name.c_str(), false))
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
}

void TokenInventory::InitTokenInstance()
{
    // 테이블에 존재하는 토큰을 모두 초기화합니다.
    _tokenTable.clear();
    if (TokenSystem* tokenSystem = GetTokenSystem())
    {
        const auto& instances = tokenSystem->GetTokenInstances();
        for (const auto& token : instances)
        {
            if (token)
            {
                _tokenTable[token->GetTokenID()] = 0;
            }
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
