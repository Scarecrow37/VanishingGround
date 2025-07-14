#pragma once
#include <Token/TokenSystem.h>

class TokenInventory
{
public:
    TokenInventory(CharacterBase* owner);
    ~TokenInventory();

public:
    /// <summary>
    /// 테이블의 토큰을 모두 제거합니다.
    /// </summary>
    void Clear();

    /// <summary>
    /// 전투가 시작될 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.
    /// </summary>
    void NotifyCombatStart();

    /// <summary>
    /// 라운드가 시작될 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.
    /// </summary>
    void NotifyRoundStart();

    /// <summary>
    /// 라운드가 끝날 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.
    /// </summary>
    void NotifyRoundEnd();

    /// <summary>
    /// 턴이 시작될 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.
    /// </summary>
    void NotifyTurnStart();  

    /// <summary>
    /// 턴이 끝날 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.
    /// </summary>
    void NotifyTurnEnd(); 

    /// <summary>
    /// CharacterBase가 Hit 당했을 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.
    /// </summary>
    void NotifyHit();

    /// <summary>
    /// CharacterBase가 사망했을 때 호출됩니다. 모든 토큰에 해당 이벤트를 알려줍니다.
    /// </summary>
    void NotifyDead();

    /// <summary>
    /// CharacterBase가 대상을 처치 시 호출됩니다.
    /// </summary>
    /// <param name="destination">처치된 대상</param>
    void NotifyKill(CharacterBase* destination);

    /// <summary>
    /// CharacterBase가 토큰을 얻었을 때 호출됩니다.
    /// </summary>
    void NotifyTokenAdded(int tokenID);

    /// <summary>
    /// CharacterBase가 토큰을 잃었을 때 호출됩니다.
    /// </summary>
    void NotifyTokenRemoved(int tokenID);

public:
    /// <summary>
    /// 토큰 스택을 카운트만큼 추가합니다. 
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void AddTokenStackFromID(int tokenID, UINT16 count = 1);

    /// <summary>
    /// <para>토큰 스택을 카운트만큼 설정합니다.</para>
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void SetTokenStackFromID(int tokenID, UINT16 count);

    /// <summary>
    /// <para>토큰 스택을 카운트만큼 제거합니다.</para>
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void RemoveTokenStackFromID(int tokenID, UINT16 count = 1);

    /// <summary>
    /// 해당 토큰의 ID로 토큰을 찾아 반환합니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <returns>해당 ID의 토큰입니다. 만약 해당 토큰이 존재하지 않으면 nullptr을 반환합니다.</returns>
    IToken* FindTokenFromID(int tokenID);

    /// <summary>
    /// 토큰을 제거합니다. 토큰이 존재하지 않으면 아무런 동작도 하지 않습니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    void RemoveTokenFromID(int tokenID);

    /// <summary>
    /// <para>해당 토큰의 ID로 해당 토큰의 카운트가 0이 아닌지 확인합니다.</para>
    /// <para>FindVaildTokenFromID보다 빠르게 동작합니다.</para>
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <returns></returns>
    bool HasToken(int tokenID) const;

    /// <summary>
    /// 유효한 토큰의 개수를 반환합니다.
    /// </summary>
    /// <returns>유효한 토큰의 개수</returns>
    size_t GetTokenCount() const;

    /// <summary>
    /// 유효한 토큰이 있는지 확인합니다.
    /// </summary>
    /// <returns>유효한 토큰이 하나라도 있으면 true, 없으면 false</returns>
    bool IsEmpty() const;

    /// <summary>
    /// ImGui 디버그 데이터를 그립니다. (토큰의 스택, 이름 등)
    /// </summary>
    void DrawImGuiDebugData();

private:
    /// <summary>
    /// <para>유효한 토큰 리스트에서 해당 토큰의 ID로 토큰을 찾아 반환합니다.</para>
    /// <para>해당 토큰이 유효한지 확인만 하는 것이면, HasToken 메서드를 사용하세요.</para>
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <returns>해당 ID의 토큰입니다. 만약 해당 토큰이 존재하지 않으면 nullptr을 반환합니다.</returns>
    IToken* FindVaildTokenFromID(int tokenID);

    /// <summary>
    /// 토큰 테이블에 모든 인스턴스를 초기화합니다.
    /// </summary>
    void InitTokenInstance();

    /// <summary>
    /// 토큰 리스트를 정렬합니다. (오름차순)
    /// </summary>
    void SortByOrder();

    /// <summary>
    /// 해당 토큰에 대한 업데이트를 수행합니다.
    /// </summary>
    /// <param tokenID="tokenID">해당 토큰의 ID</param>
    void UpdateToken(int tokenID);

    Token* FindTokenEx(int tokenID);
    Token* FindTokenEx(std::string_view tokenName);

    /// <summary>
    /// 유효한 토큰인지 확인합니다. (ex. 스택 카운트가 0이 아닌지 등)
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    bool CheckValidTokenFromID(int tokenID);

private:
    CharacterBase*                  _owner;             // 해당 매니저를 소유한 CharacterBase 인스턴스
    std::vector<Token*>             _vaildTokenVector;  // 정렬된 토큰 인스턴스 리스트
    std::vector<Token*>             _tokenInstances;    // 정렬된 토큰 인스턴스 리스트
    std::unordered_map<int, Token*> _tokenTable;        // 모든 토큰 테이블 (스택 카운트가 0인 토큰도 포함)
};