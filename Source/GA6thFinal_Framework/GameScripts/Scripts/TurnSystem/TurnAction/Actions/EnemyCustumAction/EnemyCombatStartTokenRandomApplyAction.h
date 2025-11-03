#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenRandomRangeApplyAction.h"

class EnemyCombatStartTokenRandomApplyAction : public TokenRandomRangeApplyAction
{
    USING_PROPERTY(EnemyCombatStartTokenRandomApplyAction)
public:
    EnemyCombatStartTokenRandomApplyAction()      = default;
    ~EnemyCombatStartTokenRandomApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnEnemyCombatStartPhase(Enemy& enemy) override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};