#include "pchScripts.h"
#include "GameClearState.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include "ItemDropSystem/ItemDropSystem.h"

REGISTER_CLASS(FSMStateFactory, GameClearState)

GameClearState::GameClearState() {}

GameClearState::~GameClearState() {}

void GameClearState::OnAwake() {}

void GameClearState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, u8"게임 클리어!");
    if (ItemDropSystem* system = ItemDropSystem::GetInstance())
    {
        system->PlayItemDropUISequence();
    }
}

void GameClearState::OnExit() {}

void GameClearState::OnUpdate() {}
