#include "pchScripts.h"
#include "GameClearState.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include "ItemDropSystem/ItemDropSystem.h"
#include "RevelationSystem/RevelationSystem.h"

REGISTER_CLASS(FSMStateFactory, GameClearState)

GameClearState::GameClearState() {}

GameClearState::~GameClearState() {}

void GameClearState::OnAwake() {}

void GameClearState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, u8"게임 클리어!");
    if (ItemDropSystem* system = SingletonComponent<ItemDropSystem>::GetInstance())
    {
        system->PlayItemDropUISequence();
    }
    if (_revelationSystem)
    {
        _revelationSystem->RemoveAllExtinctionElements();
    }
}

void GameClearState::OnExit() {}

void GameClearState::OnUpdate() {}
