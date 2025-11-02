#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

class MultiTokenApplyAction : public TurnAction
{
    USING_PROPERTY(MultiTokenApplyAction)
public:
    MultiTokenApplyAction();
    ~MultiTokenApplyAction() override = default;
    REFLECT_PROPERTY()

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
    std::vector<std::pair<int, int>> Tokens;
    TurnTarget TokenTarget = TurnTarget::ENEMY;
    REFLECT_FIELDS_END(MultiTokenApplyAction)

    void ImGuiDrawPropertysEvent() override;
    virtual void UpdateActionInfo() {};

    void TryTokenSystemInfoUpdate();

private:
    bool validTokenSystem = false;
};