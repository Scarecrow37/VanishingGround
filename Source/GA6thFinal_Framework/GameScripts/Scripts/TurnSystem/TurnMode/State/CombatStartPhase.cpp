#include "pchScripts.h"
#include "CombatStartPhase.h"

#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnAction/TurnAction.h>
#include <RevelationSystem/RevelationSystem.h>
#include <WeaponSystem/WeaponSystem.h>

REGISTER_CLASS(FSMStateFactory, CombatStartPhase)

CombatStartPhase::CombatStartPhase()
    : 
    _phaseEnd(false), 
    _player(nullptr)
{

}

CombatStartPhase::~CombatStartPhase() 
{

}

void CombatStartPhase::ResetCharacterStats()
{
    _player = nullptr;
    _enemies.clear();
    _characters.clear();
    for (auto& weak : GameObject::FindGameObjectsWithTag(CharacterBase::TAG))
    {
        if (false == weak.expired())
        {
            auto           object    = weak.lock();
            CharacterBase* character = nullptr;
            for (int i = 0; i < object->GetComponentCount(); ++i)
            {
                character = object->GetComponentAtIndex<CharacterBase>(i);
                if (nullptr != character)
                {
                    break;
                }
            }
            if (nullptr != character)
            {
                const auto& type = typeid(*character);
                character->Revive();
                _characters.push_back(character);
                if (typeid(Player) == type)
                {
                    _player = static_cast<Player*>(character);
                }
                else if (typeid(Enemy) == type)
                {
                    _enemies.push_back(static_cast<Enemy*>(character));
                }
            }
        }
    }
    for (auto& character : _characters)
    {
        if (character)
        {
            character->ClearState();
        }
    }
}

void CombatStartPhase::OnAwake() 
{

}

void CombatStartPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void CombatStartPhase::OnEnter() 
{
    _turnMode->ResetRoundCount();
    AddValidActions();
    ResetCharacterStats();

    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...3");
    UmTime.Invoke(&GetFSM(), 1.f, [this]() { UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...2"); });
    UmTime.Invoke(&GetFSM(), 2.f, [this]() { UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...1"); });
    UmTime.Invoke(&GetFSM(), 3.f, [this]() { this->_phaseEnd = true; });

    NotifyCombatStart();
    Battle::ResetLastCharacter();
}

void CombatStartPhase::OnExit() 
{

}

void CombatStartPhase::OnUpdate() 
{

}

void CombatStartPhase::NotifyCombatStart() 
{
    for (auto& character : _characters)
    {
        if (character)
        {
            character->OnCombatStart();
        }
    }

    _turnMode->ApplyActions([](TurnAction& action) 
    { 
         action.OnCombatStart();
    });
}


void CombatStartPhase::AddValidActions()
{
    //무기 액션들
    if (_weaponSystem)
    {
        for (auto& item : _weaponSystem->GetEquipWeapons())
        {

        }
    }
}
