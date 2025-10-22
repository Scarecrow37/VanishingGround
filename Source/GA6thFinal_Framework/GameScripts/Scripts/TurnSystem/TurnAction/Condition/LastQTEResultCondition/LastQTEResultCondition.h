#pragma once
#include <TurnSystem/TurnAction/TurnAction.h>
#include <TurnSystem/TurnAction/TurnActionEnums.h>

/// <summary>
/// 마지막 QTE의 판정 조건 입니다.
/// </summary>
class LastQTEResultCondition : public TurnActionCondition
{
    USING_PROPERTY(LastQTEResultCondition)
public:
    enum class ResultType
    {
        ALL_CRIT,       // 치명적
        OVER_HIT,       // 무결점
        ALL_CRIT_FAIL,  // 치명적 실패
        OVER_HIT_FAIL,  // 무결점 실패
    };

    LastQTEResultCondition();
    ~LastQTEResultCondition() override;

protected:
    REFLECT_FIELDS_BEGIN(TurnActionCondition)
    ResultType Type = ResultType::ALL_CRIT;
    REFLECT_FIELDS_END(LastQTEResultCondition)

    // TurnActionCondition을(를) 통해 상속됨
    bool               Evaluate() override;
    void               DrawImguiEditor() override;
    const std::string& GetConditionInfo() const override;

    void DeserializedReflectEvent() override;

private:
    void        UpdateConditionInfo();
    std::string conditionInfo;

};