#pragma once
#pragma once
#include <TurnSystem/TurnAction/TurnAction.h>

// 체력을 확인하는 조건 클래스입니다.
class HealthRangeCondition : public TurnActionCondition
{
    USING_PROPERTY(HealthRangeCondition)
public:
    // 대상
    enum class Target
    {
        SELF,
        PLAYER,
        ENEMY,
        ALL_ENEMIES,
        ALL
    };

    enum class Operator
    {
        GREATER_EQUAL, // 이상(≥A)
        LESS_EQUAL,    // 이하(≤A)
        EQUAL,         // 동일(=A)
        BETWEEN,       // 사이값(A~B)
    };

    enum class Unit
    {
        FLAT,
        PERCENT,
    };

    HealthRangeCondition() = default;
    ~HealthRangeCondition() override = default;

protected:
    bool Evaluate() override;
    void DrawImguiEditor() override;
    const std::string& GetConditionInfo() const override;

    REFLECT_FIELDS_BEGIN(TurnActionCondition)
    Target   Target   = Target::SELF;
    Operator Operator = Operator::GREATER_EQUAL;
    float    value1   = 0;
    float    value2   = 0;
    Unit     Unit     = Unit::FLAT;
    REFLECT_FIELDS_END(HealthRangeCondition)

private:
    void GetTargetList(std::vector<class CharacterBase*>& targetList);

    std::string _conditionInfo;
    void        UpdateConditionInfo();
};