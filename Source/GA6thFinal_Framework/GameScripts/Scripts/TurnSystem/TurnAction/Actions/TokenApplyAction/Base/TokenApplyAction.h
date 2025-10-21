#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

class TokenApplyAction : public TurnAction
{
    USING_PROPERTY(TokenApplyAction)
public:
    TokenApplyAction();
    ~TokenApplyAction() override = default;
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

    SETTER(int, TokenCount)
    {
        ReflectFields->TokenCount = std::max(value, 1);
        UpdateActionInfo();
    }
    GETTER(int, TokenCount) { return ReflectFields->TokenCount; }
    // 부여할 토큰 개수
    // type : int
    PROPERTY(TokenCount)

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
    int        TokenID     = 16000;
    int        TokenCount  = 1;
    TurnTarget TokenTarget = TurnTarget::ENEMY;
    REFLECT_FIELDS_END(TokenApplyAction)

    void ImGuiDrawPropertysEvent() override;
    void DeserializedReflectEvent() override;
    virtual void UpdateActionInfo() = 0;
};