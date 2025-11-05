#include "pchScripts.h"
#include "RevelationCountWeaponDamageAcation.h"
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "RevelationSystem/RevelationSystem.h"
#include "WeaponSystem/WeaponSystem.h"

REGISTER_TURN_ACTION(RevelationCountWeaponDamageAcation)

void RevelationCountWeaponDamageAcation::OnAddedAction() 
{
    if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
    {
        if (RevelationSystem* revelationSystem = SingletonComponent<RevelationSystem>::GetInstance())
        {
            //현재 무기
            auto& weapon = weaponSystem->GetCurrentWeaponElement();
            auto& stats  = weapon.Stats;

            //계시 갯수
            int count =  static_cast<int>(revelationSystem->GetPlayerElementList().size());
            _damage = count;
            _myIndex = weaponSystem->CurrentWeaponSlot;

            //증가
            stats.HitDamage += _damage;
            stats.CriticalDamage += _damage;
        }
    }
}

void RevelationCountWeaponDamageAcation::OnDestroy() 
{
    if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
    {
        // 현재 무기
        if (0 <= _myIndex && _myIndex < weaponSystem->EQUIP_WEAPONS_SIZE)
        {
            // 감소
            auto& stats = weaponSystem->GetWeaponStatsAtIndex(_myIndex);    
            stats.HitDamage -= _damage;
            stats.CriticalDamage -= _damage;
        }
    }
}

const std::string& RevelationCountWeaponDamageAcation::GetActionName()
{
    return NAME;
}

const std::string& RevelationCountWeaponDamageAcation::GetActionInfo()
{
    return NAME;
}

void RevelationCountWeaponDamageAcation::ImGuiDrawActionEditor() 
{

}
