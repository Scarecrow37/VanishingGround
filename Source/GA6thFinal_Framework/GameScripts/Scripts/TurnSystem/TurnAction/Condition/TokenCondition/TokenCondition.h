#pragma once
#include <TurnSystem/TurnAction/TurnAction.h>

/// <summary>
/// 토근 조건 확인 클래스
/// </summary>
class TokenCondition : public TurnActionCondition
{
    USING_PROPERTY(TokenCondition)
public:
    TokenCondition();
    ~TokenCondition() override = default;

    // 대상
    enum class Target
    {
        NONE,
        SELF,
        PLAYER,
        ENEMY,
        ALL_ENEMIES,
        ALL
    };

    enum class Operator
    {
        GREATER_EQUAL,
        LESS_EQUAL,
        EQUAL
    };
    

    bool Evaluate() override;
    void DrawImguiEditor() override;
    const std::string& GetConditionInfo() const override;

protected:
    REFLECT_PROPERTY()

    REFLECT_FIELDS_BEGIN(TurnActionCondition)
    Target   Target    = Target::NONE;
    int      TokenType = 16000;
    Operator Operator  = Operator::GREATER_EQUAL;
    int      Value     = 0;
    REFLECT_FIELDS_END(TokenCondition)

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    void UpdateConditionInfo();
    std::string _conditionInfo;
};
