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
    Initialize();
}

TokenInventory::~TokenInventory() 
{
    _tokenTable.clear();
    _vaildTokenVector.clear();
}

void TokenInventory::Initialize()
{
    _vaildTokenVector.clear();
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

void TokenInventory::Clear()
{
    for (auto& [id, token] : _tokenTable)
    {
        RemoveTokenFromID(id);
    }
    _vaildTokenVector.clear();
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

void TokenInventory::NotifyPrePlayerAttackCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPrePlayerAttackCalculateChain(attackerData, targetData);
        }
    });
}

void TokenInventory::NotifyPreEnemyAttackCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreEnemyAttackCalculateChain(attackerData, targetData);
        }
    });
}

void TokenInventory::NotifyPrePlayerHitCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPrePlayerHitCalculateChain(attackerData, targetData);
        }
    });
}

void TokenInventory::NotifyPreEnemyHitCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreEnemyHitCalculateChain(attackerData, targetData);
        }
    });
}

void TokenInventory::NotifyPostPlayerAttackCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                      int& chain)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPostPlayerAttackCalculateChain(attackerData, targetData, chain);
        }
    });
}

void TokenInventory::NotifyPostEnemyAttackCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                     int& chain)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPostEnemyAttackCalculateChain(attackerData, targetData, chain);
        }
    });
}

void TokenInventory::NotifyPostPlayerHitCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData, int& chain)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPostPlayerHitCalculateChain(attackerData, targetData, chain);
        }
    });
}

void TokenInventory::NotifyPostEnemyHitCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData, int& chain)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPostEnemyHitCalculateChain(attackerData, targetData, chain);
        }
    });
}

void TokenInventory::NotifyPrePlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPrePlayerAttackCalculateDamage(attackerData, targetData);
        }
    });
}

void TokenInventory::NotifyPreEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreEnemyAttackCalculateDamage(attackerData, targetData);
        }
    });
}

void TokenInventory::NotifyPrePlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPrePlayerHitCalculateDamage(attackerData, targetData);
        }
    });
}

void TokenInventory::NotifyPreEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPreEnemyHitCalculateDamage(attackerData, targetData);
        }
    });
}

void TokenInventory::NotifyPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                       int& damage)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPostPlayerAttackCalculateDamage(attackerData, targetData, damage);
        }
    });
}

void TokenInventory::NotifyPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                      int& damage)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPostEnemyAttackCalculateDamage(attackerData, targetData, damage);
        }
    });
}

void TokenInventory::NotifyPostPlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                    int& damage)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPostPlayerHitCalculateDamage(attackerData, targetData, damage);
        }
    });
}

void TokenInventory::NotifyPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                   int& damage)
{
    NotifyTokenEvent([&](Token& token) {
        bool valid = HasTokenFromID(token.GetTokenID());
        if (valid)
        {
            token.OnPostEnemyHitCalculateDamage(attackerData, targetData, damage);
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
        if (IToken* token = GetTokenFromID(tokenID))
        {
            int  maxStackCount = token->GetMaxStackCount();
            int& curStackCount = it->second;
            if (curStackCount >= maxStackCount)
            {   // 이미 최대 스택에 도달했으면 추가하지 않습니다.(이벤트를 호출하지 않기 위해 필요)
                return; 
            }
            if (token->CanAdd(&_owner))
            {
                curStackCount += count;
                curStackCount = std::min(maxStackCount, curStackCount);
                UpdateToken(tokenID);
                _owner.OnTokenAdded(tokenID);
                std::string msg = std::format("{}{}{}{}{}{}", 
                    _owner.gameObject->ToString(),
                    (const char*)u8" 에게 ",
                    token->GetTokenName(),
                    (const char*)u8" 토큰이 ",
                    count,
                    (const char*)u8"개 부여되었습니다."
                );
                UmLogger.Log(LogLevel::LEVEL_DEBUG, msg);
            }
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
        if (IToken* token = GetTokenFromID(tokenID))
        {
            int& curStackCount = it->second;
            if (curStackCount <= 0)
            { // 스택이 0 이하이면 제거하지 않습니다. (이벤트를 호출하지 않기 위해 필요)
                return;
            }
            if (token->CanRemove(&_owner))
            {
                curStackCount -= count;
                curStackCount = std::max(0, curStackCount);
                UpdateToken(tokenID);
                _owner.OnTokenRemoved(tokenID);
                std::string msg = std::format("{}{}{}{}{}{}", 
                    _owner.gameObject->ToString(),
                    (const char*)u8"에게 ",
                    token->GetTokenName(),
                    (const char*)u8" 토큰이 ",
                    count,
                    (const char*)u8"개 제거되었습니다."
                );
                UmLogger.Log(LogLevel::LEVEL_DEBUG, msg);
            }
        }
    }
}

void TokenInventory::RemoveTokenFromID(int tokenID)
{
    if (_tokenTable.contains(tokenID))
    {
        int& count = _tokenTable[tokenID];
        if (0 < count)
        {
            RemoveTokenStackFromID(tokenID, count);
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
    size_t count = 0;
    if (TokenSystem* tokenSystem = GetTokenSystem())
    {
        if (auto* set = tokenSystem->GetTokenIDSetFromTag(tag))
        {
            for (auto& id : *set)
            {
                if (HasTokenFromID(id))
                {
                    ++count;
                }
            }
        }
    }
    return count > 0;
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

int TokenInventory::GetTokenStackFromTag(const std::string& tag) const
{
    int count = 0;
    if (TokenSystem* tokenSystem = GetTokenSystem())
    {
        if (auto* set = tokenSystem->GetTokenIDSetFromTag(tag))
        {
            for (auto& id : *set)
            {
                count += GetTokenStackFromID(id);
            }
        }
    }
    return count;
}

int TokenInventory::GetValidTokenCount() const
{
    return static_cast<int>(_vaildTokenVector.size());
}

int TokenInventory::GetValidTokenCount(const std::string& tag) const
{
    int count = 0;
    if (TokenSystem* tokenSystem = GetTokenSystem())
    {
        if (auto* set = tokenSystem->GetTokenIDSetFromTag(tag))
        {
            for (auto& id : *set)
            {
                count += HasTokenFromID(id) ? 1 : 0;
            }
        }
    }
    return count;
}

int TokenInventory::GetValidTokenCountByTag() const
{
    int count = 0;
    if (TokenSystem* tokenSystem = GetTokenSystem())
    {
        const auto& tagTable = tokenSystem->GetTokenTagTable();
        for (const auto& [tag, idSet] : tagTable)
        {
            if (HasTokenFromTag(tag))
            {
                ++count;
            }
        }
    }
    return count;
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
