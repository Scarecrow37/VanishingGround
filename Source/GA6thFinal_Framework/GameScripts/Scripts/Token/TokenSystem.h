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
    void OnDrawDebug() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void RegisterAllTokenInstance();
    // 테이블에 추가
    void RegisterTokenInstanceToTable(Token* token);
    // 테이블에서 제거
    void UnregisterTokenInstanceToTable(Token* token);

    void LoadTokenDataFromExcelData(ExcelDataSystem* dataSystem);

    void ImGuiDrawDataTable();
    void ImGuiDrawMenuBar();

public:

    /// <summary>
    /// TokenInstance를 ID로 가져옵니다.
    /// </summary>
    /// <param name="tokenID">Token의 ID값</param>
    /// <returns>해당 ID에 맞는 IToken 인스턴스 포인터</returns>
    static IToken* GetTokenFromID(int tokenID);

    /// <summary>
    /// TokenInstance를 이름으로 가져옵니다.
    /// </summary>
    /// <param name="name">Token의 Name값</param>
    /// <returns>해당 Name에 맞는 IToken 인스턴스 포인터</returns>
    static IToken* GetTokenFromName(std::string_view name);

    /// <summary>
    /// 토큰 이름을 통해 토큰 ID를 가져옵니다.
    /// </summary>
    /// <param name="tokenID">해당 토큰의 ID값</param>
    /// <returns>해당 Token의 Name값. 존재하지 않는 ID라면 빈 문자열을 반환합니다.</returns>
    static const std::string& GetTokenNameFromID(int tokenID);

    /// <summary>
    /// Token ID를 통해 토큰 이름을 가져옵니다.
    /// </summary>
    /// <param name="tokenName">해당 토큰의 Name값</param>
    /// <returns>해당 토큰의 Name값. 존재하지 않는 이름이라면 -1을 반환합니다.</returns>
    static int GetTokenIDFromName(std::string_view tokenName);

    /// <summary>
    /// 정렬되어있는 토큰 리스트입니다.
    /// </summary>
    static inline const std::vector<Token*>&  GetTokenInstances() { return _tokenInstances; }

    /// <summary>
    /// 정렬되어있는 토큰 리스트입니다.
    /// </summary>
    static inline const std::vector<Token*>& GetTokenInstancesFromTag(TokenTag tag) { return _tokenTagTable[tag]; }

public:
    /// <summary>
    /// 토큰을 등록합니다. 직접 호출하지 않고 define을 통해 등록합니다.
    /// </summary>
    /// <typeparam name="T">Token 클래스 타입입니다.</typeparam>
    /// <returns>등록 성공 여부입니다.</returns>
    template <typename T>
    static bool RegisterTokenFactory();

private:    
    /////////////////////////////////////////////////////////////
    // !!! Internal only !!!
    /////////////////////////////////////////////////////////////
    /// <summary>Token을 ID로 가져옵니다.</summary>
    static Token* GetTokenFromIDEx(int tokenID);
    /// <summary>Token을 이름으로 가져옵니다.</summary>
    static Token* GetTokenFromNameEx(std::string_view name);
    /// <summary> 토큰 리스트를 정렬합니다.(오름차순) </summary>
    static void SortByOrder();
    /// <summary>토큰 ID를 통해 토큰 인스턴스를 생성합니다.</summary>
    static bool CreateTokenInstanceFromID(int tokenID, Token** ppToken);
    /// <summary>토큰 Name을 통해 토큰 인스턴스를 생성합니다.</summary>
    static bool CreateTokenInstanceFromName(std::string_view tokenName, Token** ppToken);

private:
    SingletonComponent<TokenSystem> _singletonComponent{this};
    bool    _isOpenEditor = false;
    Token*  _selectedToken = nullptr;
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<int, std::string> TokenSerializeData;
    REFLECT_FIELDS_END(TokenSystem)

    // Runtime token type information
    inline static std::vector<Token*>                                       _tokenInstances;        // 등록된 토큰 인스턴스 리스트
    inline static std::unordered_map<int, Token*>                           _tokenIDTable;          // 토큰 ID별로 분류된 토큰 리스트
    inline static std::unordered_map<std::string, Token*>                   _tokenNameTable;        // 토큰 이름별로 분류된 토큰 리스트
    inline static std::unordered_map<TokenTag, std::vector<Token*>>         _tokenTagTable;         // 토큰 태그별로 분류된 토큰 리스트
    inline static std::unordered_map<std::string, std::function<Token*()>>  _tokenNameFactoryTable; // 토큰 이름별로 토큰 생성 팩토리 함수
    inline static std::unordered_map<std::string, int>                      _tokenNameToIDTable;    // 토큰 이름과 ID를 매핑한 테이블
    inline static std::unordered_map<int, std::string>                      _tokenIDToNameTable;    // 토큰 ID와 이름을 매핑한 테이블
    

    std::unordered_map<TokenID, TokenData> _tokenDataTable; // 토큰 데이터 테이블
    inline static std::unordered_map<int, std::function<Token*()>>          _tokenIDFactoryTable;   // 토큰 ID별로 토큰 생성 팩토리 함수
};

template <typename T>
inline bool TokenSystem::RegisterTokenFactory()
{
    static_assert(std::is_base_of_v<Token, T>, "T must be derived from Token");
    std::function<Token*()> factoryFunc = []() { return new T(); };
    int                     ID          = T::ID;
    std::string             name        = (const char*)T::NAME;
    auto                    idIter      = _tokenIDFactoryTable.find(ID);
    auto                    nameIter    = _tokenNameFactoryTable.find(name);
    if (idIter == _tokenIDFactoryTable.end())
    {
        _tokenIDFactoryTable[ID] = factoryFunc;
    }
    else
    {
        assert(false && "토큰 등록 중 ID 충돌이 발생했습니다.");
        return false;
    }
    if (nameIter == _tokenNameFactoryTable.end())
    {
        _tokenNameFactoryTable[name] = factoryFunc;
    }
    else
    {
        assert(false && "토큰 등록 중 Name 충돌이 발생했습니다.");
        return false;
    }
    _tokenNameToIDTable[name] = ID;
    _tokenIDToNameTable[ID]   = name;
    return true;
}
