#pragma once
#include "Interface/IToken.h"

// @brief 토큰의 기본 멤버입니다. 이걸 추가하지 않으면 System에 추가가 불가능합니다
#define TOKEN_DATA(id, name)                                                    \
public:                                                                         \
static constexpr int ID = id;                                                   \
static constexpr const char8_t* NAME = u8##name;                                \
inline int         GetTokenID() const  override { return ID; }                  \
inline const char* GetTokenName() const override { return (const char*)NAME; }  \
using Data = reflect_fields_struct;

// @brief 토큰을 등록하는 매크로입니다. 이걸 사용하지 않으면 토큰이 System에 등록되지 않습니다.
// #include<Token/TokenSystem.h>을 포함해야합니다.
#define REGISTER_TOKEN(CLASS)                                           \
namespace TokenRegister  {                                              \
namespace CLASS##Register  {                                            \
        static bool IsRegister = TokenSystem::RegisterToken<CLASS>();   \
    }}                                                       

class Token : public ReflectSerializer, public IToken
{
    USING_PROPERTY(Token)
  
public:
    Token();
    virtual ~Token();

public: // 콜백에 대한 자세한 주석은 ITriggerType.h를 참고하세요.
    virtual void OnCombatStart(CharacterBase* source) override                                  {};
    virtual void OnRoundStart(CharacterBase* owner) override                                    {};
    virtual void OnRoundEnd(CharacterBase* owner) override                                      {};
    virtual void OnTurnStart(CharacterBase* owner) override                                     {};
    virtual void OnTurnEnd(CharacterBase* owner) override                                       {};
    virtual void OnHit(CharacterBase* owner) override                                           {};
    virtual void OnDead(CharacterBase* owner) override                                          {};
    virtual void OnKill(CharacterBase* source, CharacterBase* destination) override             {};
    virtual void OnTokenAdded(CharacterBase* owner, int tokenID) override                       {};
    virtual void OnTokenRemoved(CharacterBase* owner, int tokenID) override                     {};
    virtual void OnQTEStart(CharacterBase* owner) override                                      {};
    virtual void OnQTEEnd(CharacterBase* owner) override                                        {};

public:
    UINT16  GetStackCount() const override;
    UINT16  GetMaxStackCount() const override;
    int     GetTokenOrder() const;
    void    ClearStack();
    void    SetStack(UINT16 count);
    void    AddStack(UINT16 count = 1);
    void    RemoveStack(UINT16 count = 1);
    void    SetMaxStackCount(UINT16 maxStack);
    void    SetDirtyCountCallback(std::function<void(int)> callback);
    void    SetDirtyOrderCallback(std::function<void(int)> callback);
    void    SetTokenOrder(int order);

protected:
    UINT16  _stackCount = 0;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    // 토큰의 실행 우선 순위
    inline static int    Order = 0;
    // 토큰의 최대 스택 수
    inline static UINT16 MaxStackCount = UINT16_MAX;
    REFLECT_FIELDS_END(Token)

    std::function<void(int)> _dirtyCountCallback; // 스택이 변경되었을 때 호출되는 콜백 함수
    std::function<void(int)> _dirtyOrderCallback; // 스택이 변경되었을 때 호출되는 콜백 함수
};