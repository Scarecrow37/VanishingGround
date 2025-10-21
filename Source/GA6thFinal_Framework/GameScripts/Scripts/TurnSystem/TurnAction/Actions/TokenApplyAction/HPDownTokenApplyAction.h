#pragma once
#include <TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h>

class HPDownTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(HPDownTokenApplyAction)
public:
    HPDownTokenApplyAction() = default;
    ~HPDownTokenApplyAction() override = default;

public:
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;


private:
    void UpdateActionInfo() override;
    std::string _actionInfo;

};