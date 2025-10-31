#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

//아무 일도 일어나지 않음
class NotingAction : public TurnAction
{
protected:
    // TurnAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

private:
    inline static const std::string name = (const char*)u8"아무일도 일어나지 않음";
};