#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"

class RecoveryByTokenCountAction : public TurnAction
{
public:
    inline static const std::string NAME = (const char*)u8"죽인 대상의 특정 토큰 갯수만큼 회복";
    RecoveryByTokenCountAction() = default;
    ~RecoveryByTokenCountAction() override = default;

protected:
    REFLECT_FIELDS_BEGIN(TurnAction)
    TurnTarget Target  = TurnTarget::PLAYER;
    int        TokenID = 0;
    REFLECT_FIELDS_END(RecoveryByTokenCountAction)
private:
    void OnEnemyDeadByWeapon(Enemy& enemy, WeaponElement& weapon) override;
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
};