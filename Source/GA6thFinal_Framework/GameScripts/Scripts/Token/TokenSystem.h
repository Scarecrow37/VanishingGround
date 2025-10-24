#pragma once
#include <Token/Common/TokenCommon.h>
#include <Token/Token.h>
#include <Token/Object/Bleed/BleedToken.h>
#include <Token/Object/Poison/PoisonToken.h>
#include <Utility/SingletonHelper.h>

// @brief 토큰을 등록하는 매크로입니다. 이걸 사용하지 않으면 토큰이 System에 등록되지 않습니다.
// @brief Include :
// @brief <Token/TokenSystem.h> 혹은 <Token/TokenInventory.h> 을 포함해야합니다.
#define REGISTER_TOKEN(CLASS)                                                       \
    namespace TokenRegister                                                         \
    {                                                                               \
        namespace CLASS##Register                                                   \
        {                                                                           \
            static bool IsRegister = TokenSystem::RegisterTokenFactory<CLASS>();    \
        }                                                                           \
    }   

class IToken;
class Token;
class ExcelDataSystem;

/// <summary>
/// 토큰 시스템은 엑셀을 통해 토큰 정보들을 매핑하고 인스턴스를 관리합니다.
/// 싱글톤 패턴을 사용하여 전역에서 접근할 수 있습니다.
/// DontDestroyOnLoad로 설정되어 있습니다.
/// </summary>
class TokenSystem : public Component
{
    USING_PROPERTY(TokenSystem)

public:
    TokenSystem();
    ~TokenSystem();

private:
    void Reset() override; 
    void Added() override;
    void Awake() override;
    void OnDestroy() override;
    void ImGuiDrawPropertysEvent() override;

public:
    IToken* GetTokenFromID(TokenID tokenID);
    const TokenData* GetTokenDataFromID(TokenID tokenID);
    const std::string& GetTokenNameFromID(TokenID tokenID);
    const std::set<TokenID>* GetTokenIDSetFromTag(const std::string& tag);

    /// <summary> 정렬되어있는 토큰 리스트입니다. </summary>
    inline const std::vector<std::unique_ptr<Token>>&  GetTokenInstances() { return _tokenInstances; }
    inline const std::unordered_map<std::string, std::set<TokenID>>& GetTokenTagTable() { return _tokenTagTable; }

private:
    void Clear();
    void SortByOrder();
    void RegisterAllTokenInstance();
    void RegisterTokenInstanceToTable(Token* token);
    void UnregisterTokenInstanceToTable(Token* token);
    void LoadTokenDataFromExcelData(ExcelDataSystem* dataSystem);

private:
    SingletonObject<TokenSystem>    _singletonObject{this};
    SingletonComponent<TokenSystem> _singletonComponent{this};

    std::map<TokenID, TokenData>                            _tokenDataTable; // 토큰 데이터 테이블
    std::vector<std::unique_ptr<Token>>                     _tokenInstances; // 토큰 인스턴스 리스트
    std::map<TokenID, Token*>                               _tokenIDTable;   // 토큰 ID별로 토큰 인스턴스 매핑 테이블
    std::unordered_map<std::string, std::set<TokenID>>      _tokenTagTable;  // 토큰 태그별로 토큰 ID 매핑 테이블

    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TokenSystem)

public: 
    /////////////////////////////////////////////////////////
    // Factory 관련
    /////////////////////////////////////////////////////////
    
    /// <summary>
    /// 토큰을 등록합니다. 직접 호출하지 않고 define을 통해 등록합니다.
    /// </summary>
    /// <typeparam name="T">Token 클래스 타입입니다.</typeparam>
    /// <returns>등록 성공 여부입니다.</returns>
    template <typename T>  requires std::is_base_of_v<Token, T>
    static bool RegisterTokenFactory();

    static const std::vector<TokenID>& GetRegisteredTokenList() { return _registeredFactoryList; }

private:
    inline static std::vector<TokenID> _registeredFactoryList;
    inline static std::unordered_map<TokenID, std::function<Token*()>> _registeredFactoryTable;   // 토큰 ID별로 토큰 생성 팩토리 함수

public:
    static const char*  TokenIDToName(TokenID tokenID);
    static const char*  TokenIDToTag(TokenID tokenID);
    static int          TokenIDToOrder(TokenID tokenID);
    static int          TokenIDToMaxStack(TokenID tokenID);
};

template <typename T> requires std::is_base_of_v<Token, T>
inline bool TokenSystem::RegisterTokenFactory()
{
    if (_registeredFactoryTable.contains(T::ID))
    {
        assert(false && "동일한 토큰 ID가 이미 등록되어 있습니다.");
        return false;
    }
    std::function<Token*()> factoryFunc = []() { return new T(); };
    _registeredFactoryTable[T::ID] = factoryFunc;
    _registeredFactoryList.push_back(T::ID);
    return true;
}
