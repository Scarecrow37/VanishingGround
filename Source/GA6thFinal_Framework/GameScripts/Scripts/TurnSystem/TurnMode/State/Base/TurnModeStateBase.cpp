 #include "pchScripts.h"
#include "TurnModeStateBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include <WeaponSystem/WeaponSystem.h>
#include <RevelationSystem/RevelationSystem.h>    

TurnModeStateBase::TurnModeStateBase() {}

TurnModeStateBase::~TurnModeStateBase() {}

void TurnModeStateBase::OnStart() 
{
    _turnMode         = SingletonComponent<TurnMode>::GetInstance();
    _weaponSystem     = SingletonComponent<WeaponSystem>::GetInstance();
    _revelationSystem = SingletonComponent<RevelationSystem>::GetInstance();
    if (_turnMode == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Game System에 Turn Mode가 존재하지 않습니다.");
    }
    if (_weaponSystem == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Game System에 Weapon System이 존재하지 않습니다.");
    }
    if (_revelationSystem == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Game System에 Revelation System이 존재하지 않습니다.");
    }
}
