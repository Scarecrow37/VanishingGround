#pragma once
#include "TurnSystem/TurnAction/Actions/TokenApplyAction/Base/TokenApplyAction.h"
#include "WeaponSystem/WeaponElement/WeaponElement.h"

// 특정 타입의 무기로 특정 횟수만큼 공격시 캐릭터에게 토큰 부여
class WeaponAttackTypeCountTokenApplyAction : public TokenApplyAction
{
    USING_PROPERTY(WeaponAttackTypeCountTokenApplyAction)

    inline static const std::string NAME = (const char*)u8"특정 타입의 무기로 특정 횟수만큼 공격시 캐릭터에게 토큰 부여";

public:
    WeaponAttackTypeCountTokenApplyAction() = default;
    ~WeaponAttackTypeCountTokenApplyAction() override = default;

protected:
    REFLECT_PROPERTY(ReflectFields->WeaponTargetType,ReflectFields->TargetAttackCount)

    REFLECT_FIELDS_BEGIN(TokenApplyAction)
    WeaponType WeaponTargetType  = WeaponType::SWORD;
    int        TargetAttackCount = 10;
    REFLECT_FIELDS_END(WeaponAttackTypeCountTokenApplyAction)

    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
    const std::string& GetActionName() override;

    void OnEquipAccessory() override;

    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                               Enemy& target, EnemyStats& targetStats) override;

private:
    int _attackCount = 0;

};