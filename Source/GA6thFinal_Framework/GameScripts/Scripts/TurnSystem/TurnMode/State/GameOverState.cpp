#include "pchScripts.h"
#include "GameOverState.h"

REGISTER_CLASS(FSMStateFactory, GameOverState)

GameOverState::GameOverState() {}

GameOverState::~GameOverState() {}

void GameOverState::OnAwake() {}

void GameOverState::OnStart()
{
    TurnModeStateBase::OnStart();
}

void GameOverState::OnEnter() 
{
    UmLogger.Log(LogLevel::LEVEL_DEBUG, (const char*)u8"게임 오버!!!!");
}

void GameOverState::OnExit() 
{

}

void GameOverState::OnUpdate() 
{

}
