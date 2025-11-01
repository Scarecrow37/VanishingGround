#pragma once
#include <TurnSystem/TurnAction/TurnAction.h>
#include <TurnSystem/TurnAction/TurnActionEnums.h>

//연격 확인 조건
class ChainCondition : public TurnActionCondition
{
    USING_PROPERTY(ChainCondition)
public:
    enum class Operator
    {
        GREATER_EQUAL, //이상(≥A)
        LESS_EQUAL,    //이하(≤A)
        EQUAL,         //동일(=A)
        BETWEEN,       //사이값(A~B). 유일하게 Value2 항목을 사용한다.
        MULTIPLE_OF    //배수달성(A의연격)
    };

    ChainCondition();
    ~ChainCondition() override = default;

    bool Evaluate() override;
    void DrawImguiEditor() override;
    const std::string& GetConditionInfo() override;

    REFLECT_PROPERTY()
    REFLECT_FIELDS_BEGIN(TurnActionCondition)
    TurnTarget Target   = TurnTarget::SELF;
    Operator   Operator = Operator::GREATER_EQUAL;
    int        Value1   = 0;
    int        Value2   = 0;
    REFLECT_FIELDS_END(ChainCondition)

protected:
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    std::string _conditionInfo;
    void        UpdateConditionInfo();

};