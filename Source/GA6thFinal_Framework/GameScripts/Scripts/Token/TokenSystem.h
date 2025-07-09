#pragma once
#include "Token.h"

class TokenSystem
{
public:
    TokenSystem()  = default;
    ~TokenSystem() = default;

public:
    template<typename T>
    static bool RegisterToken();

    /// <summary>
    /// 테이블의 토큰을 모두 제거합니다.
    /// </summary>
    void Clear();

    /// <summary>
    /// 라운드가 시작될 때 호출됩니다. 모든 토큰에 대해 OnRoundStart를 호출합니다.
    /// </summary>
    /// <param name="owner"></param>
    void OnRoundStart(CharacterBase* owner);

    /// <summary>
    /// 라운드가 끝날 때 호출됩니다. 모든 토큰에 대해 OnRoundEnd를 호출합니다.
    /// </summary>
    /// <param name="owner"></param>
    void OnRoundEnd(CharacterBase* owner);

    /// <summary>
    /// 턴이 시작될 때 호출됩니다. 모든 토큰에 대해 OnTurnStart를 호출합니다.
    /// </summary>
    /// <param name="owner"></param>
    void OnTurnStart(CharacterBase* owner);  

    /// <summary>
    /// 턴이 끝날 때 호출됩니다. 모든 토큰에 대해 OnTurnEnd를 호출합니다.
    /// </summary>
    /// <param name="owner"></param>
    void OnTurnEnd(CharacterBase* owner); 

    /// <summary>
    /// CharacterBase가 Hit 당했을 때 호출됩니다. 모든 토큰에 대해 OnHit를 호출합니다.
    /// </summary>
    /// <param name="owner"></param>
    void OnHit(CharacterBase* owner);

public:
    /// <summary>
    /// 토큰 스택 카운트를 추가합니다. 만약 해당 토큰이 존재하지 않는다면 새로 생성합니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void AddTokenStackFromID(int tokenID, UINT8 count);

    /// <summary>
    /// 토큰 스택 카운트를 설정합니다. 만약 해당 토큰이 존재하지 않는다면 새로 생성합니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void SetTokenStack(int tokenID, UINT8 count);

    /// <summary>
    /// 토큰 스택 카운트를 제거합니다. 만약 토큰이 존재하지 않으면 아무런 동작도 하지 않습니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <param name="count">제거할 카운트 수</param>
    void RemoveTokenStack(int tokenID, UINT8 count);

    /// <summary>
    /// 해당 토큰의 ID로 토큰을 찾아 반환합니다. 만약 해당 토큰이 존재하지 않으면 nullptr을 반환합니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <returns></returns>
    IToken* FindToken(int tokenID);

private:
    /// <summary>
    /// Token을 ID로 찾아 반환합니다.시스템 내부에서만 사용하는 함수입니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    /// <returns></returns>
    Token* FindTokenEx(int tokenID);

    /// <summary>
    /// 유효한 토큰인지 확인합니다. (ex. 스택 카운트가 0이 아닌지 등)
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID</param>
    void CheckValidToken(int tokenID);

    /// <summary>
    /// 토큰 ID를 통해 토큰 인스턴스를 생성합니다.
    /// </summary>
    /// <param name="tokenID">생성할 토큰의 ID</param>
    /// <returns>생성된 토큰의 주소 값</returns>
    Token* CreateTokenInstanceFromID(int tokenID);

    /// <summary>
    /// 토큰 Name를 통해 토큰 인스턴스를 생성합니다.
    /// </summary>
    /// <param name="tokenName">생성할 토큰의 Name</param>
    /// <returns>생성된 토큰의 주소 값</returns>
    Token* CreateTokenInstanceFromName(std::string_view tokenName);

private:
    std::unordered_map<int, Token*> _tokenTable;

    inline static std::unordered_map<int, std::function<Token*()>> _tokenIDFactoryTable;
    inline static std::unordered_map<std::string, std::function<Token*()>> _tokenNameFactoryTable;
};

template <typename T>
inline static bool TokenSystem::RegisterToken()
{
    static_assert(std::is_base_of_v<Token, T>, "T must be derived from Token");
    std::function<Token*()> factoryFunc = []() { return new T(); };
    auto idIter = _tokenIDFactoryTable.find(T::ID);
    auto nameIter = _tokenNameFactoryTable.find(T::NAME);
    if (idIter == _tokenIDFactoryTable.end())
    {
        _tokenIDFactoryTable[T::ID] = factoryFunc;
    }
    else
    {
        assert(false && "토큰 등록 중 ID 충돌이 발생했습니다.");
        return false;
    }
    if (nameIter == _tokenNameFactoryTable.end())
    {
        _tokenNameFactoryTable[T::NAME] = factoryFunc;
    }
    else
    {
        assert(false && "토큰 등록 중 Name 충돌이 발생했습니다.");
        return false;
    }
    return true;
}