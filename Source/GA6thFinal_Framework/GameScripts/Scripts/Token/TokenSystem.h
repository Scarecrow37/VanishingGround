#pragma once
#include <Token/Token.h>
#include <Token/Object/Bleed/BleedToken.h>
#include <Token/Object/Poison/PoisonToken.h>

// @brief 토큰을 등록하는 매크로입니다. 이걸 사용하지 않으면 토큰이 System에 등록되지 않습니다.
// @brief Include :
// @brief <Token/TokenSystem.h> 혹은 <Token/TokenInventory.h> 을 포함해야합니다.
#define REGISTER_TOKEN(CLASS)                                                   \
    namespace TokenRegister                                                     \
    {                                                                           \
        namespace CLASS##Register                                               \
        {                                                                       \
            static bool IsRegister = TokenSystem::RegisterToken<CLASS>();       \
        }                                                                       \
    }   

class IToken;
class Token;

class TokenSystem : public Component
{
    USING_PROPERTY(TokenSystem)
    inline static TokenSystem* _staticInstance;

public:
    TokenSystem();
    ~TokenSystem();
    inline static TokenSystem* GetInstance() { return _staticInstance; }

private:
    void Reset() override; 
    void OnDrawDebug() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void ImGuiDrawDataTable();
    void ImGuiDrawMenuBar();

public:
    /// <summary>
    /// 토큰 ID를 통해 토큰 인스턴스를 생성합니다.
    /// </summary>
    /// <param name="tokenID">생성할 토큰의 ID</param>
    /// <returns>생성된 토큰의 주소 값</returns>
    static bool CreateTokenInstanceFromID(int tokenID, Token** ppToken);

    /// <summary>
    /// 토큰 Name을 통해 토큰 인스턴스를 생성합니다.
    /// </summary>
    /// <param name="tokenName">생성할 토큰의 Name</param>
    /// <returns>생성된 토큰의 주소 값</returns>
    static bool CreateTokenInstanceFromName(std::string_view tokenName, Token** ppToken);

    /// <summary>
    ///
    /// </summary>
    /// <param name="tokenID"></param>
    /// <returns></returns>
    static const std::string& GetTokenNameFromID(int tokenID);

    /// <summary>
    /// 
    /// </summary>
    /// <param name="tokenName"></param>
    /// <returns></returns>
    static int GetTokenIDFromName(std::string_view tokenName);

    /// <summary>
    /// 토큰 이름과 ID를 매핑한 테이블입니다.
    /// </summary>
    static inline const std::unordered_map<std::string, int>& GetTokenNameToIDTable() { return _tokenNameToIDTable; }

    /// <summary>
    /// 토큰 ID와 테이블을 매핑한 테이블입니다.
    /// </summary>
    static inline const std::unordered_map<int, std::string>& GetTokenIDToNameTable() { return _tokenIDToNameTable; }

    /// <summary>
    /// 정렬되어있는 토큰 리스트입니다.
    /// </summary>
    static inline const std::vector<Token*>&  GetTokenInstances() { return _tokenInstances; }

public:
    /// <summary>
    /// 토큰을 등록합니다. 직접 호출하지 않고 define을 통해 등록합니다.
    /// </summary>
    /// <typeparam name="T">Token 클래스 타입입니다.</typeparam>
    /// <returns>등록 성공 여부입니다.</returns>
    template <typename T>
    static bool RegisterToken();

    static Token* GetTokenFromID(int tokenID);
    static Token* GetTokenFromName(std::string_view name);

private:
    /// <summary>
    /// 토큰 리스트를 정렬합니다. (오름차순)
    /// </summary>
    static void SortByOrder();

private:
    bool _isOpenEditor = false;
    Token* _selectedToken = nullptr;
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<int, std::string> TokenSerializeData;
    REFLECT_FIELDS_END(TokenSystem)

    // Runtime token type information
    inline static std::vector<Token*>                                       _tokenInstances;
    inline static std::unordered_map<int, Token*>                           _tokenIDTable; 
    inline static std::unordered_map<std::string, Token*>                   _tokenNameTable; 
    inline static std::unordered_map<int, std::function<Token*()>>          _tokenIDFactoryTable;
    inline static std::unordered_map<std::string, std::function<Token*()>>  _tokenNameFactoryTable;
    inline static std::unordered_map<std::string, int>                      _tokenNameToIDTable;
    inline static std::unordered_map<int, std::string>                      _tokenIDToNameTable;
};

template <typename T>
inline bool TokenSystem::RegisterToken()
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
    T* newToken = new T();
    newToken->SetDirtyOrderCallback([](int id) { SortByOrder(); });
    _tokenInstances.push_back(newToken);
    _tokenIDTable[ID]         = newToken;
    _tokenNameTable[name]     = newToken;
    _tokenNameToIDTable[name] = ID;
    _tokenIDToNameTable[ID]   = name;
    return true;
}
