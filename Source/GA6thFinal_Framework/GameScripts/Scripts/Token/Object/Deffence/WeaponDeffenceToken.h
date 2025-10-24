#pragma once
#include <Token/Token.h>
#include <Stats/Weapon/WeaponStats.h>

namespace TokenObject
{
    // 무기 방어 토큰 (플레이어가 공격할 때만 적용)
    class WeaponDeffence : public Token
    {
    private:
        void OnRoundEnd(CharacterBase* owner) override;
        void OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                           int& damage) override;

    protected:
        virtual WeaponType GetDeffenceWeaponType() const = 0;
    };
    class SwordDeffence1 : public WeaponDeffence
    {
        TOKEN_DATA(205037)
        inline WeaponType GetDeffenceWeaponType() const override { return WeaponType::SWORD; }
    };
    class SwordDeffence2 : public WeaponDeffence
    {
        TOKEN_DATA(205038)
        inline WeaponType GetDeffenceWeaponType() const override { return WeaponType::SWORD; }
    };
    class DaggerDeffence1 : public WeaponDeffence
    {
        TOKEN_DATA(205039)
        inline WeaponType GetDeffenceWeaponType() const override { return WeaponType::DAGGER; }
    };
    class DaggerDeffence2 : public WeaponDeffence
    {
        TOKEN_DATA(205040)
        inline WeaponType GetDeffenceWeaponType() const override { return WeaponType::DAGGER; }
    };
    class HammerDeffence1 : public WeaponDeffence
    {
        TOKEN_DATA(205041)
        inline WeaponType GetDeffenceWeaponType() const override { return WeaponType::WARHAMMER; }
    };
    class HammerDeffence2 : public WeaponDeffence
    {
        TOKEN_DATA(205042)
        inline WeaponType GetDeffenceWeaponType() const override { return WeaponType::WARHAMMER; }
    };
}