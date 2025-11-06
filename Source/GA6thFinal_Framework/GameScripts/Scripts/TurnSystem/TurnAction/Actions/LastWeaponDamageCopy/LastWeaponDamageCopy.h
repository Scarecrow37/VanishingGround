#pragma once
#include "TurnSystem/TurnAction/TurnActionFactory.h"

class LastWeaponDamageCopy : public TurnAction
{
public:
    static const std::string& GetActionNameStr();

protected:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void ImGuiDrawActionEditor() override;

    void OnTurnStart(CharacterBase& destination) override;
};
