#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"

class EnemyStunTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(EnemyStunTokenApplyAction)
public:
    EnemyStunTokenApplyAction()           = default;
    ~EnemyStunTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnEnemyStun(Enemy& enemy) override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};
