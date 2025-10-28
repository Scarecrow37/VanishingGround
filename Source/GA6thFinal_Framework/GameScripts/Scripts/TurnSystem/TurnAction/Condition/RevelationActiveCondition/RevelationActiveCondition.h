#pragma once
#include <TurnSystem/TurnAction/TurnAction.h>
#include <TurnSystem/TurnAction/TurnActionEnums.h>

class RevelationActiveCondition : public TurnActionCondition
{
    USING_PROPERTY(RevelationActiveCondition)
public:
    RevelationActiveCondition();
    ~RevelationActiveCondition() override;


protected:
    bool Evaluate() override;
    void DrawImguiEditor() override;
    const std::string& GetConditionInfo() override;

};