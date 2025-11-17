#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"

class TurnEndTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(TurnEndTokenApplyAction)
public:
    TurnEndTokenApplyAction()           = default;
    ~TurnEndTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnTurnEnd(CharacterBase& destination) override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};
