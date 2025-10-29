#pragma once
#include <TurnSystem/TurnAction/TurnAction.h>

//라운드당 한번만 True를 반환하는 조건 클래스 입니다.
class RoundOnceTrueCondition : public TurnActionCondition
{
    friend class TurnMode;
    USING_PROPERTY(RoundOnceTrueCondition)
public:
    RoundOnceTrueCondition()       = default;
    ~RoundOnceTrueCondition() override = default;

    bool Evaluate() override;
    void DrawImguiEditor() override;
    const std::string& GetConditionInfo() override;
    
    void OnEvaluateConditions(bool result) override;

public:
    struct RoundOnceAction : public TurnAction
    {
        friend class TurnMode;
        RoundOnceAction();
        ~RoundOnceAction() override;

        bool Once = false;

        const std::string& GetActionName() override;
        const std::string& GetActionInfo() override;
        void               ImGuiDrawActionEditor() override;
        void               OnRoundStart() override;
    private:
        inline static const std::string NAME = "RoundOnceCondition";
        inline static std::unordered_set<RoundOnceAction*> _roundOnceActions; //라운드 스타트를 감지를 위해 TurnMode가 초기화 될때 이 액션들을 등록해줍니다.
    };

private:
    RoundOnceAction _roundOnceAction;
};

