#pragma once
#include "TurnSystem/TurnAction/TurnAction.h"
#include "WeaponSystem/WeaponElement/WeaponElement.h"

class WeaponTypeCountAttackDamageAction : public TurnAction
{
    USING_PROPERTY(WeaponTypeCountAttackDamageAction)
public:
    inline static const std::string NAME = (const char*)u8"무기 종류 갯수 만큼 해당 타입의 무기 데미지 증가";

    WeaponTypeCountAttackDamageAction() = default;
    ~WeaponTypeCountAttackDamageAction() override = default;

protected:
    REFLECT_PROPERTY(
        ReflectFields->Type,
        ReflectFields->HitDamage,
        ReflectFields->CriticalDamage
    )

    REFLECT_FIELDS_BEGIN(TurnAction)
    WeaponType Type = WeaponType::SWORD;
    int HitDamage = 0;
    int CriticalDamage = 0;
    REFLECT_FIELDS_END(WeaponTypeCountAttackDamageAction)

    int CaculateHitDamage();
    int CaculateCriticalDamage();

    void OnPlayerBattleCalculateDamageModifier(Player& attacker, PlayerStats& attackerStats,
                                                    WeaponStats& weaponStats, Enemy& target,
                                                    EnemyStats& targetStats) override;

    void OnConvertWeaponViewModel(WeaponStats& stats) override;

    // TurnAction을(를) 통해 상속됨
    const std::string& GetActionName() override;
    const std::string& GetActionInfo() override;
    void               ImGuiDrawActionEditor() override;
};