#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/MultiTokenApplyAction.h"

// 무기 공격으로 적 처치시 토큰 부여
class WeaponKillMultiTokenApplyAction : public MultiTokenApplyAction
{
public:
    WeaponKillMultiTokenApplyAction() = default;
    ~WeaponKillMultiTokenApplyAction() override = default;

protected:
    // TokenApplyAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;

    void OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) override;

private:
    void        UpdateActionInfo() override;
    std::string _actionInfo;
};