#pragma once
#include <Token/Common/TokenCommon.h>
#include <Token/TokenSystem.h>

namespace QTE
{
    struct NoteResult;
}

class TokenInventory
{
    using TokenID = int;
public:
    TokenInventory(CharacterBase* owner);
    ~TokenInventory();

public:
    void Initialize();

    /// <summary>
    /// 테이블의 토큰을 모두 제거합니다.
    /// </summary>
    void Clear();

    /// <summary>전투가 시작될 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.</summary>
    void NotifyCombatStart();

    /// <summary>라운드가 시작될 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.</summary>
    void NotifyRoundStart();

    /// <summary>라운드가 끝날 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.</summary>
    void NotifyRoundEnd();

    /// <summary>라운드가 끝날 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.</summary>
    void NotifyEachTurnStart(CharacterBase* destination);

    /// <summary>턴이 시작될 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.</summary>
    void NotifyTurnStart();  

    /// <summary>턴이 끝날 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.</summary>
    void NotifyTurnEnd(); 

    /// <summary>CharacterBase가 Hit 당했을 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.</summary>
    void NotifyHit();

    /// <summary>CharacterBase가 사망했을 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.</summary>
    void NotifyDead();

    /// <summary>CharacterBase가 대상을 처치 시 호출됩니다.</summary>
    /// <param name="destination">처치된 대상</param>
    void NotifyKill(CharacterBase* destination);

    /// <summary>CharacterBase가 토큰 스택을 얻었을 때 호출됩니다.</summary>
    void NotifyTokenAdded(int tokenID);

    /// <summary>CharacterBase가 토큰 스택을 잃었을 때 호출됩니다.</summary>
    void NotifyTokenRemoved(int tokenID);

    /// <summary>CharacterBase가 토큰이 생겼을 때 호출됩니다.</summary>
    void NotifyTokenEnter(int tokenID);

    /// <summary>CharacterBase가 토큰이 없어졌을 때 호출됩니다.</summary>
    void NotifyTokenExit(int tokenID);

    /// <summary>QTE가 시작될 때 호출됩니다.</summary>
    void NotifyQTEStart();

    /// <summary> QTE가 끝날 때 호출됩니다.</summary>
    void NotifyQTEEnd();

    void NotifyPrePlayerAttackCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData);
    void NotifyPreEnemyAttackCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData);
    void NotifyPrePlayerHitCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData);
    void NotifyPreEnemyHitCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData);

    void NotifyPostPlayerAttackCalculateChain(PlayerAttackData& attackerData, EnemyHitData&  targetData, int& chain);
    void NotifyPostEnemyAttackCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData, int& chain);
    void NotifyPostPlayerHitCalculateChain(EnemyAttackData& attackerData, PlayerHitData& targetData, int& chain);
    void NotifyPostEnemyHitCalculateChain(PlayerAttackData& attackerData, EnemyHitData& targetData, int& chain);

    void NotifyPrePlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData);
    void NotifyPreEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData);
    void NotifyPrePlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData);
    void NotifyPreEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData);

    void NotifyPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage);
    void NotifyPostEnemyAttackCalculateDamage(EnemyAttackData&  attackerData, PlayerHitData& targetData, int& damage);
    void NotifyPostPlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage);
    void NotifyPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage);


    void NotifyRollRandomSpeed(int& randomSpeed);

public:
    /// <summary>
    /// 토큰 스택을 카운트만큼 추가합니다. 
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void AddTokenStackFromID(int tokenID, int count = 1);

    /// <summary>
    /// <para>토큰 스택을 카운트만큼 설정합니다.</para>
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void SetTokenStackFromID(int tokenID, int count);

    /// <summary>
    /// <para>토큰 스택을 카운트만큼 제거합니다.</para>
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void RemoveTokenStackFromID(int tokenID, int count = 1);

    /// <summary>
    /// 토큰을 제거합니다. 토큰이 존재하지 않으면 아무런 동작도 하지 않습니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    void RemoveTokenFromID(int tokenID);

    /// <summary>
    /// <para>해당 토큰의 ID로 해당 토큰의 카운트가 0이 아닌지 확인합니다.</para>
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <returns></returns>
    bool HasTokenFromID(int tokenID) const;

    /// <summary>
    /// 지정된 태그에 해당하는 토큰이 있는지 확인합니다.
    /// </summary>
    /// <param name="tokenTag">확인할 TokenTag 값입니다.</param>
    /// <returns>태그에 해당하는 토큰이 있으면 true, 없으면 false를 반환합니다.</returns>
    bool HasTokenFromTag(const std::string& tag) const;

    /// <summary>
    /// 토큰 ID로 해당 토큰의 스택 카운트를 반환합니다.
    /// </summary>
    int GetTokenStackFromID(int tokenID) const;

    /// <summary>
    /// 지정된 태그에 대응하는 모든 토큰의 스택을 합한 카운트를 반환합니다.
    /// </summary>
    int GetTokenStackFromTag(const std::string& tag) const;

    /// <summary>
    /// 유효한 토큰의 개수를 반환합니다.
    /// </summary>
    int GetValidTokenCount() const;

    /// <summary>
    /// 해당 태크에 해당하는 유효한 토큰의 개수를 반환합니다.
    /// </summary>
    int GetValidTokenCount(const std::string& tag) const;

    /// <summary>
    /// 유효한 토큰 종류의 개수를 반환합니다.
    /// </summary>
    int GetValidTokenCountByTag() const;

    /// <summary>
    /// 유효한 토큰이 하나라도 있는지 확인합니다.
    /// </summary>
    bool IsEmpty() const;
    
    /// <summary>
    /// ImGui 디버그 데이터를 그립니다. (토큰의 스택, 이름 등)
    /// </summary>
    void DrawImGuiDebugData();

    inline const std::vector<TokenID>& GetValidTokenList() const { return _vaildTokenVector; }

private:
    /// <summary>
    /// 해당 토큰에 대한 업데이트를 수행합니다.
    /// </summary>
    /// <param tokenID="tokenID">해당 토큰의 ID</param>
    void UpdateToken(TokenID tokenID);

    /// <summary>
    /// 유효한 토큰인지 확인합니다. (ex. 스택 카운트가 0이 아닌지 등)
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    bool CheckValidTokenFromID(TokenID tokenID);

private:
    CharacterBase&                      _owner;              // 해당 매니저를 소유한 CharacterBase 인스턴스
    std::vector<TokenID>                _vaildTokenVector;   // 유효한 토큰 ID 리스트(쌓인 순서대로)
    std::unordered_map<TokenID, int>    _tokenTable;         // 모든 토큰 테이블 (스택 카운트가 0인 토큰도 포함)
};