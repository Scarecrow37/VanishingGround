#include "pchScripts.h"
#include "TurnModeStateBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include <WeaponSystem/WeaponSystem.h>
#include <RevelationSystem/RevelationSystem.h>    

TurnModeStateBase::TurnModeStateBase() {}

TurnModeStateBase::~TurnModeStateBase() {}

void TurnModeStateBase::OnStart() 
{
    _turnMode = GetFSM().GetComponent<TurnMode>();
    _weaponSystem = GetFSM().GetComponent<WeaponSystem>();
    if (_weaponSystem == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Game System에 Weapon System이 존재하지 않습니다.");
    }
    _revelationSystem = GetFSM().GetComponent<RevelationSystem>();
    if (_revelationSystem == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Game System에 Revelation System이 존재하지 않습니다.");
    }
}
