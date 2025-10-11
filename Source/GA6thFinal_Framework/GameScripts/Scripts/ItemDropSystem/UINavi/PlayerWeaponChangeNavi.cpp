#include "pchScripts.h"
#include "PlayerWeaponChangeNavi.h"
#include "UI/Elements/Image/ImageElement.h"
#include "WeaponSystem/WeaponSystem.h"
#include "ItemDropSystem/UI/WeaponChangeUIManager.h"

UMREAL_COMPONENT(PlayerWeaponChangeNavi)

void PlayerWeaponChangeNavi::Awake()
{
    Base::Awake();
    _focusImage = GetComponent<ImageElement>();
}

void PlayerWeaponChangeNavi::FocusIn(FocusCallType type)
{
    Base::FocusIn(type);
    if (_focusImage)
    {
        _focusImage->Enable = true;
    }
    if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
    {
        if (WeaponChangeUIManager* uiManager = SingletonComponent<WeaponChangeUIManager>::GetInstance())
        {
            auto& weapons = weaponSystem->GetEquipWeapons();
            if (_weaponIndex < weapons.size())
            {
                uiManager->SetPlayerWeaponStatsUI(weapons[_weaponIndex]);
            }
        }
    }
}

void PlayerWeaponChangeNavi::Submit() 
{
    Base::Submit();
    if (WeaponChangeUIManager* manager = SingletonComponent<WeaponChangeUIManager>::GetInstance())
    {
        if (false == manager->HasWarningUI())
        {
            manager->ShowChangeWarningUI((int)_weaponIndex);
        }
    }
}

void PlayerWeaponChangeNavi::FocusOut(FocusCallType type) 
{
    Base::FocusOut(type);
    if (_focusImage)
    {
        _focusImage->Enable = false;
    }
}
