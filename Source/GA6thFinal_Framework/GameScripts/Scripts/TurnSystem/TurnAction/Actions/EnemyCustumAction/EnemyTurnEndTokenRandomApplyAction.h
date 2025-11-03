#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenRandomRangeApplyAction.h"

class EnemyTurnEndTokenRandomApplyAction : public TokenRandomRangeApplyAction
{
    USING_PROPERTY(EnemyTurnEndTokenRandomApplyAction)
public:
    EnemyTurnEndTokenRandomApplyAction()               = default;
    ~EnemyTurnEndTokenRandomApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnEnemyTurnEnd(Enemy& enemy) override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};

class EnemyTurnEndTokenRandomSetAction : public TokenRandomRangeApplyAction
{
    USING_PROPERTY(EnemyTurnEndTokenRandomSetAction)
public:
    EnemyTurnEndTokenRandomSetAction()             = default;
    ~EnemyTurnEndTokenRandomSetAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnEnemyTurnEnd(Enemy& enemy) override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};
