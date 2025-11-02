#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

//TODO: 효과 구현은 아직 안됨
// 플레이어의 토큰 데미지 증가
class TokenDamaggeAction : public TurnAction
{
    USING_PROPERTY(TokenDamaggeAction)
public:
    inline static const std::string NAME = (const char*)u8"플레이어가 부여한 토큰의 데미지 증가";

    TokenDamaggeAction()         = default;
    ~TokenDamaggeAction() override = default;

    REFLECT_PROPERTY(Damage)
    SETTER(int, TokenID)
    {
        ReflectFields->TokenID = std::max(value, 0);
    }
    GETTER(int, TokenID) { return ReflectFields->TokenID; }
    // 대상 토큰 ID
    // type : int
    PROPERTY(TokenID)

    SETTER(int, Damage)
    {
        ReflectFields->Damage = value;
    }
    GETTER(int, Damage) { return ReflectFields->Damage; }
    // 증가할 데미지
    // type : int
    PROPERTY(Damage)

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    int        TokenID     = 0;
    int        Damage      = 0;
    REFLECT_FIELDS_END(TokenDamaggeAction)

    void TryTokenSystemInfoUpdate();

private:
    bool validTokenSystem = false;

    // TurnAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void ImGuiDrawPropertysEvent() override;
};