#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"

class RoundStartTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(RoundStartTokenApplyAction)
public:
    RoundStartTokenApplyAction()        = default;
    ~RoundStartTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnRoundStart() override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};
