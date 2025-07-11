#pragma once
#include <Token/TokenSystem.h>

class TokenManager
{
public:
    TokenManager(CharacterBase* owner);
    ~TokenManager() = default;

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
    /// 해당 토큰의 ID로 토큰을 찾아 반환합니다. 만약 해당 토큰이 존재하지 않으면 nullptr을 반환합니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <returns></returns>
    IToken* FindTokenFromID(int tokenID);

    /// <summary>
    /// 토큰을 제거합니다. 토큰이 존재하지 않으면 아무런 동작도 하지 않습니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    void RemoveTokenFromID(int tokenID);

    bool HasToken(int tokenID) const;

    bool IsEmpty() const;

private:
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
    /// <param name="token"></param>
    void UpdateToken(int tokenID);

    Token* FindTokenEx(int tokenID);
    Token* FindTokenEx(std::string_view tokenName);

    /// <summary>
    /// 유효한 토큰인지 확인합니다. (ex. 스택 카운트가 0이 아닌지 등)
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    bool CheckValidTokenFromID(int tokenID);

private:
    CharacterBase* _owner;
    std::vector<Token*>             _tokenInstances;    // 토큰 인스턴스 리스트
    std::unordered_map<int, Token*> _tokenTable;        // 모든 토큰 테이블 (스택 카운트가 0인 토큰도 포함)
    std::unordered_map<int, Token*> _validTokenTable;   // 유효한 토큰 테이블 (스택 카운트가 0이 아닌 토큰만 포함)
};