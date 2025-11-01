 #include "pchScripts.h"
#include "TurnModeStateBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "WeaponSystem/WeaponSystem.h"
#include "RevelationSystem/RevelationSystem.h"   
#include "AccessorySystem/AccessorySystem.h"
#include "TurnSystem/TurnMode/State/CombatStartPhase.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/CharacterStats.h"

TurnModeStateBase::TurnModeStateBase() {}

TurnModeStateBase::~TurnModeStateBase() {}

void TurnModeStateBase::OnStart() 
{
    _turnMode         = SingletonComponent<TurnMode>::GetInstance();
    _weaponSystem     = SingletonComponent<WeaponSystem>::GetInstance();
    _revelationSystem = SingletonComponent<RevelationSystem>::GetInstance();
    _accessorySystem  = SingletonComponent<AccessorySystem>::GetInstance();
    if (_turnMode == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Player System에 Turn Mode가 존재하지 않습니다.");
    }
    if (_weaponSystem == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Player System에 Weapon System이 존재하지 않습니다.");
    }
    if (_revelationSystem == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Player System에 Revelation System이 존재하지 않습니다.");
    }
    if (_accessorySystem == nullptr)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Player System에 Accessory System이 존재하지 않습니다.");
    }
}

void TurnModeStateBase::UpdateCharacterDead(const std::function<void(CharacterBase&)>& deadCallback)
{
    CombatStartPhase* combatStartPhase = _turnMode->States->CombatStartPhase;
    if (combatStartPhase)
    {
        bool isDead = false;
        for (auto& character : combatStartPhase->GetCharacters())
        {
            if (false == character->IsDead())
            {
                int hp = character->HP;
                if (hp <= 0)
                {
                    character->Dead();
                    if (deadCallback)
                    {
                        deadCallback(*character);
                    }
                    isDead = true;
                }
            }
        }

        if (isDead)
        {
            _turnMode->EraseTurnListToDeadCharacter();
        }
    }
}

void TurnModeStateBase::ApplyReduceHP()
{
    if (_turnMode)
    {
        const auto& characters = _turnMode->GetCharacters();
        for (auto character : characters)
        {
            if (character)
            {
                if (auto* stats = character->GetCharacterStats())
                {
                    stats->ApplyReduce();
                }
            }
        }
    }
}
