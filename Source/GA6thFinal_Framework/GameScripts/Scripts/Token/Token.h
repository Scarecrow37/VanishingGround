#pragma once
#include "Interface/IToken.h"

// @brief 토큰의 기본 멤버입니다. 이걸 추가하지 않으면 System에 추가가 불가능합니다
#define TOKEN_DATA(id, name)\
public:\
static constexpr int ID = id;\
static constexpr const char* NAME = name;

// @brief 토큰을 등록하는 매크로입니다. 이걸 사용하지 않으면 토큰이 System에 등록되지 않습니다.
#define REGISTER_TOKEN(CLASS)\
namespace TokenRegister  {                                                                                  \
namespace CLASS##Register  {                                                                                \
        static bool IsRegister = TokenSystem::RegisterToken<CLASS>();                                       \
    }}                                                       

class Token : public IToken
{
public:
    Token() = default;
    virtual ~Token() = default;

public:
    virtual void OnRoundStart(CharacterBase* owner) override = 0;
    virtual void OnRoundEnd(CharacterBase* owner)   override = 0;
    virtual void OnTurnStart(CharacterBase* owner)  override = 0;
    virtual void OnTurnEnd(CharacterBase* owner)    override = 0;
    virtual void OnHit(CharacterBase* owner)        override = 0;

public:
    UINT8   GetStackCount() const override;
    UINT8   GetMaxStackCount() const override;
    void    ClearStack();
    void    SetStack(UINT8 count);
    void    AddStack(UINT8 count = 1);
    void    RemoveStack(UINT8 count = 1);
    void    SetMaxStackCount(UINT8 maxStack);

private:
    UINT8   _stackCount = 0;
    UINT8   _maxStackCount = UINT8_MAX;
};