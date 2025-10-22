#pragma once
#include <Token/Common/TokenCommon.h>
#include <Token/Token.h>
#include <Token/Object/Bleed/BleedToken.h>
#include <Token/Object/Poison/PoisonToken.h>
#include <Utility/SingletonHelper.h>

// @brief 토큰을 등록하는 매크로입니다. 이걸 사용하지 않으면 토큰이 System에 등록되지 않습니다.
// @brief Include :
// @brief <Token/TokenSystem.h> 혹은 <Token/TokenInventory.h> 을 포함해야합니다.
#define REGISTER_TOKEN(CLASS)                                                   \
    namespace TokenRegister                                                     \
    {                                                                           \
        namespace CLASS##Register                                               \
        {                                                                       \
            static bool IsRegister = TokenSystem::RegisterTokenFactory<CLASS>();       \
        }                                                                       \
    }   

class IToken;
class Token;
class ExcelDataSystem;

/// <summary>
/// 토큰 시스템은 게임 내에서 사용되는 토큰인스턴스를 공유해주는 시스템입니다.
/// 토큰의 ID와 이름을 통해 토큰 인스턴스 정보를 매핑합니다.
/// 싱글톤 패턴을 사용하여 전역에서 접근할 수 있습니다.
/// </summary>
class TokenSystem : public Component
{
    USING_PROPERTY(TokenSystem)

public:
    TokenSystem();
    ~TokenSystem();

private:
    void Reset() override; 
    void Awake() override;
    void OnDestroy() override;
    void ImGuiDrawPropertysEvent() override;

public:
    /// <summary> TokenID를 통해 IToken을 가져옵니다. </summary>
    IToken* GetTokenFromID(TokenID tokenID);

    const std::string& GetTokenNameFromID(TokenID tokenID);

    /// <summary> TokenID를 통해 토큰 데이터를 가져옵니다. </summary>
    const TokenData* GetTokenDataFromID(TokenID tokenID);

    const std::set<Token*>* GetTokenInstancesFromTag(const std::string& tag);

    /// <summary> 정렬되어있는 토큰 리스트입니다. </summary>
    inline const std::vector<std::unique_ptr<Token>>&  GetTokenInstances() { return _tokenInstances; }

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

    std::unordered_map<TokenID, TokenData>                  _tokenDataTable; // 토큰 데이터 테이블
    std::vector<std::unique_ptr<Token>>                     _tokenInstances; // 토큰 인스턴스 리스트
    std::unordered_map<TokenID,     Token*>                 _tokenIDTable;
    std::unordered_map<std::string, Token*>                 _tokenNameTable;
    std::unordered_map<std::string, std::set<Token*>>       _tokenTagTable;

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

    /// <summary>토큰 ID를 통해 토큰 인스턴스를 생성합니다.</summary>
    static bool CreateTokenInstanceFromID(int tokenID, Token** ppToken);

private:
    inline static std::unordered_map<TokenID, std::function<Token*()>> _tokenIDFactoryTable;   // 토큰 ID별로 토큰 생성 팩토리 함수

};

template <typename T> requires std::is_base_of_v<Token, T>
inline bool TokenSystem::RegisterTokenFactory()
{
    if (_tokenIDFactoryTable.contains(T::ID))
    {
        assert(false); // [assert] 동일한 토큰 ID가 이미 등록되어 있습니다.
        return false;
    }
    std::function<Token*()> factoryFunc = []() { return new T(); };
    _tokenIDFactoryTable[T::ID] = factoryFunc;
    return true;
}
