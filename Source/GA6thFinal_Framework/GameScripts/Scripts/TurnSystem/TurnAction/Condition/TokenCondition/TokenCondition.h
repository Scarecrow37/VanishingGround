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

    enum class Operator
    {
        GREATER_EQUAL,
        LESS_EQUAL,
        EQUAL
    };
    

    bool Evaluate() override;
    void DrawImguiEditor() override;
    const std::string& GetConditionInfo() override;

protected:
    REFLECT_PROPERTY()

    REFLECT_FIELDS_BEGIN(TurnActionCondition)
    TurnTarget Target    = TurnTarget::SELF;
    int        TokenType = 16000;
    Operator   Operator  = Operator::GREATER_EQUAL;
    int        Value     = 0;
    REFLECT_FIELDS_END(TokenCondition)

private:
    void UpdateConditionInfo();
    std::string _conditionInfo;

    void GetTargetList(std::vector<class CharacterBase*>& targetList);

    void TryUpdateTokenSystemInfo();

    bool _validTokenSystem = false;
};
