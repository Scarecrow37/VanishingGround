#pragma once
#include "UmFramework.h"

//턴 액션 조건 재사용을 위한 Base 클래스
class TurnActionCondition : public ReflectSerializer
{
    USING_PROPERTY(TurnActionCondition)
public:
    //2개 이상의 조건끼리의 연산을 정의합니다.
    enum class LogicalOperator
    {
        AND,
        OR
    };

    TurnActionCondition() = default;
    ~TurnActionCondition() override = default;

    /// <summary>
    /// 조건 검사 함수
    /// </summary>
    /// <returns></returns>
    virtual bool Evaluate() = 0;

    /// <summary>
    /// 에디터용 ImGui Draw를 요청합니다. 
    /// </summary>
    virtual void DrawImguiEditor() = 0;

    /// <summary>
    /// Condition을 설명하는 문자열 반환합니다.
    /// </summary>
    virtual const std::string& GetConditionInfo() = 0;

    GETTER(int, Order) { return ReflectFields->Order; }
    SETTER(int, Order) { ReflectFields->Order = value; }
    PROPERTY(Order)

    GETTER(TurnActionCondition::LogicalOperator, LogicOperator) { return ReflectFields->LogicOperator; }
    SETTER(TurnActionCondition::LogicalOperator, LogicOperator) { ReflectFields->LogicOperator = value; }
    PROPERTY(LogicOperator)

protected:
    REFLECT_PROPERTY(Order, LogicOperator)

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    int                                  Order         = 0;
    TurnActionCondition::LogicalOperator LogicOperator = LogicalOperator::AND;
    REFLECT_FIELDS_END(TurnActionCondition)

private:

};