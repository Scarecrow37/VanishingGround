#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"

class CombatStartTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(CombatStartTokenApplyAction)
public:
    CombatStartTokenApplyAction()          = default;
    ~CombatStartTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnCombatStart() override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};