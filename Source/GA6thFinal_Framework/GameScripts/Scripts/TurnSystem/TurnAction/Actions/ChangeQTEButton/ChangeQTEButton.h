#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

class ChangeQTEButton : public TurnAction
{
    USING_PROPERTY(ChangeQTEButton)
public:
    ChangeQTEButton();
    ~ChangeQTEButton() override;

protected:
    // TurnAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnAddedAction() override;
    void OnDestroy() override;

private:
    void UpdateActionInfo();
    std::string _actionInfo;
};
