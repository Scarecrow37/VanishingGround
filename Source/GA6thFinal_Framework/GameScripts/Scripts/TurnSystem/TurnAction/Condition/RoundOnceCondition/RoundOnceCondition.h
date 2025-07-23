#pragma once
#include <TurnSystem/TurnAction/TurnAction.h>

//라운드당 한번만 True를 반환하는 조건 클래스 입니다.
class RoundOnceTrueCondition : public TurnActionCondition
{
    friend class TurnAction;
    USING_PROPERTY(RoundOnceTrueCondition)
public:
    RoundOnceTrueCondition()       = default;
    ~RoundOnceTrueCondition() override = default;

    bool Evaluate() override;
    void DrawImguiEditor() override;
    const std::string& GetConditionInfo() const override;
    
    void OnEvaluateConditions(bool result) override;

private:
    struct RoundOnceAction : public TurnAction
    {
        RoundOnceAction();
        ~RoundOnceAction() override;

        bool Once = false;

        const std::string& GetActionName() override;
        const std::string& GetActionInfo() override;
        void               ImGuiDrawActionEditor() override;
        void               OnRoundStart() override;
    private:
        inline static const std::string NAME = "RoundOnceCondition";
    };
    RoundOnceAction _roundOnceAction;
};

