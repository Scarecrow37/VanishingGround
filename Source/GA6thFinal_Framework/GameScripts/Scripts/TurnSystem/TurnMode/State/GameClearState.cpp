#include "pchScripts.h"
#include "GameClearState.h"

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
