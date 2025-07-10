#pragma once
#include "Interface/IToken.h"

// @brief 토큰의 기본 멤버입니다. 이걸 추가하지 않으면 System에 추가가 불가능합니다
#define TOKEN_DATA(id, name)                \
public:                                     \
static constexpr int ID = id;               \
static constexpr const char* NAME = name;

// @brief 토큰을 등록하는 매크로입니다. 이걸 사용하지 않으면 토큰이 System에 등록되지 않습니다.
// #include<Token/TokenSystem.h>을 포함해야합니다.
#define REGISTER_TOKEN(CLASS)                                           \
namespace TokenRegister  {                                              \
namespace CLASS##Register  {                                            \
        static bool IsRegister = TokenSystem::RegisterToken<CLASS>();   \
    }}                                                       

class Token : public IToken
{
public:
    Token() = default;
    virtual ~Token() = default;

public: // 콜백에 대한 자세한 주석은 ITriggerType.h를 참고하세요.
    virtual void OnRoundStart(CharacterBase* owner) override                        = 0;
    virtual void OnRoundEnd(CharacterBase* owner) override                          = 0;
    virtual void OnTurnStart(CharacterBase* owner) override                         = 0;
    virtual void OnTurnEnd(CharacterBase* owner) override                           = 0;
    virtual void OnHit(CharacterBase* owner) override                               = 0;
    virtual void OnDead(CharacterBase* owner) override                              = 0;
    virtual void OnKill(CharacterBase* source, CharacterBase* destination) override = 0;
    virtual void OnTokenAdded(CharacterBase* owner, int tokenID) override                        = 0;
    virtual void OnTokenRemoved(CharacterBase* owner, int tokenID) override                      = 0;

public:
    UINT16  GetStackCount() const override;
    UINT16  GetMaxStackCount() const override;
    void    ClearStack();
    void    SetStack(UINT16 count);
    void    AddStack(UINT16 count = 1);
    void    RemoveStack(UINT16 count = 1);
    void    SetMaxStackCount(UINT16 maxStack);

private:
    UINT16  _stackCount = 0;
    UINT16  _maxStackCount = UINT16_MAX;
};