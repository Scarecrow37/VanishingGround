#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

class TokenRandomRangeApplyAction : public TurnAction
{
    USING_PROPERTY(TokenRandomRangeApplyAction)
public:
    TokenRandomRangeApplyAction();
    ~TokenRandomRangeApplyAction() override = default;
    REFLECT_PROPERTY()

    SETTER(int, TokenID)
    {
        ReflectFields->TokenID = std::max(value, 0);
        UpdateActionInfo();
    }
    GETTER(int, TokenID) { return ReflectFields->TokenID; }
    // 부여할 토큰 ID
    // type : int
    PROPERTY(TokenID)

    SETTER(int, TokenCountMin)
    {
        ReflectFields->TokenCountMin = std::max(value, 1);
        UpdateActionInfo();
    }
    GETTER(int, TokenCountMin) { return ReflectFields->TokenCountMin; }
    // 부여할 토큰 최소 개수
    // type : int
    PROPERTY(TokenCountMin)

    SETTER(int, TokenCountMax)
    {
        ReflectFields->TokenCountMax = std::max(value, 1);
        UpdateActionInfo();
    }
    GETTER(int, TokenCountMax) { return ReflectFields->TokenCountMax; }
    // 부여할 토큰 최대 개수
    // type : int
    PROPERTY(TokenCountMax)

    SETTER(TurnTarget, Target)
    {
        ReflectFields->TokenTarget = value;
        UpdateActionInfo();
    }
    GETTER(TurnTarget, Target) { return ReflectFields->TokenTarget; }
    // 토큰을 부여할 대상을 설정합니다.
    // type : TurnTarget
    PROPERTY(Target)

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    int        TokenID          = 0;
    int        TokenCountMin    = 1;
    int        TokenCountMax    = 1;
    TurnTarget TokenTarget      = TurnTarget::ENEMY;
    REFLECT_FIELDS_END(TokenRandomRangeApplyAction)

    void         ImGuiDrawPropertysEvent() override;
    virtual void UpdateActionInfo() {};

    void TryTokenSystemInfoUpdate();

private:
    bool validTokenSystem = false;
};
