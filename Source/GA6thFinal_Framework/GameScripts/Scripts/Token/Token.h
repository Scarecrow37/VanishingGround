#pragma once
#include "Interface/IToken.h"

// @brief 토큰의 기본 멤버입니다. 이걸 추가하지 않으면 System에 추가가 불가능합니다
#define TOKEN_DATA(id, key)                                                     \
public:                                                                         \
static constexpr int ID = id;                                                   \
static constexpr const char8_t* NAME = u8##key;                                 \
inline int         GetTokenID() const  override { return ID; }                  \
inline const char* GetTokenName() const override { return (const char*)NAME; }  \
using Data = reflect_fields_struct;                                             \
void ShowReflectFieldView() override {                                          \
    const auto view = rfl::to_view(*ReflectFields);                             \
    view.apply([&](const auto& rflField) {                                      \
        ImGui::Text("%s", rflField.name().data());                              \
    });                                                                         \
}

#define TOKEN_CONSTRUCTOR(className, order, maxStack)                           \
 USING_PROPERTY(className)                                                      \
    className() {                                                               \
    SetTokenOrder(order);                                                       \
    SetMaxStackCount(maxStack);                                                 \
    }

class Token : public ReflectSerializer, public IToken
{
    USING_PROPERTY(Token)
  
public:
    Token();
    virtual ~Token();
    REFLECT_PROPERTY(Order)

    GETTER(int, Order) { return ReflectFields->Order; }
    SETTER(int, Order) { SetTokenOrder(value); }
    PROPERTY(Order)
    // GETTER_ONLY(int, MaxStackCount) { return static_cast<int>(MaxStackCount); }
    // PROPERTY(MaxStackCount)

    virtual void ShowReflectFieldView() = 0;

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
    int  GetMaxStackCount() const override;
    int  GetTokenOrder() const;
    void SetMaxStackCount(UINT16 maxStack);
    void SetTokenOrder(int order);
    void SetDirtyOrderCallback(std::function<void(int)> callback);

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    // 토큰의 실행 우선 순위
    int Order = 0;
    // 토큰의 최대 스택 수
    int MaxStackCount = 99;
    REFLECT_FIELDS_END(Token)

    std::function<void(int)> _dirtyOrderCallback = nullptr; // 우선순위가 변경되었을 때 호출되는 콜백 함수
};