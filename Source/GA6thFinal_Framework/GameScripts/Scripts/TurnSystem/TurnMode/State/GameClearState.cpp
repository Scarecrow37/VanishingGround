#include "pchScripts.h"
#include "GameClearState.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>

REGISTER_CLASS(FSMStateFactory, GameClearState)

GameClearState::GameClearState() {}

GameClearState::~GameClearState() {}

void GameClearState::OnAwake() {}

void GameClearState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, u8"게임 클리어!");
}

void GameClearState::OnExit() {}

void GameClearState::OnUpdate() {}
