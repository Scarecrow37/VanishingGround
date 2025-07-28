#pragma once
#include <TurnSystem/TurnAction/TurnActionFactory.h>

class MultiTargetDamageAction : public TurnAction
{
    USING_PROPERTY(MultiTargetDamageAction)
public:
    MultiTargetDamageAction();
    ~MultiTargetDamageAction() override = default;
    REFLECT_PROPERTY()
        
protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    REFLECT_FIELDS_END(MultiTargetDamageAction)


private:
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    void               OnPlayerBattleTargetSelected(Battle::EnemyTargetFlag& targetFlag) override;

    void UpdateActionInfo();
    std::string _actionInfo;
};

