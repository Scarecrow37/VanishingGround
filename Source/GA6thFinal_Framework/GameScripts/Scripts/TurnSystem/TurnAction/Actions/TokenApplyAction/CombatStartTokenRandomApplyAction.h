#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenRandomRangeApplyAction.h"

class CombatStartTokenRandomApplyAction : public TokenRandomRangeApplyAction
{
    USING_PROPERTY(CombatStartTokenRandomApplyAction)
public:
    CombatStartTokenRandomApplyAction() = default;
    ~CombatStartTokenRandomApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;
    void               OnEnemyCombatStartPhase(CharacterBase& character) override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};