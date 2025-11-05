#include "pchScripts.h"
#include "GameOverState.h"
#include "GameOverManager/GameOverManager.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnMode/TurnMode.h"

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
    UmTime.Invoke(GetFSM(), 2.0f, [this]() {
        if (_turnMode)
        {
            if (Player* player = _turnMode->GetPlayer())
            {
                if (false == player->gameObject->ActiveSelf)
                {
                    if (GameOverManager* manager = SingletonComponent<GameOverManager>::GetInstance())
                    {
                        manager->ProcessGameOver();
                    }
                }
            }
        }
    });
}

void GameOverState::OnExit() 
{
}

void GameOverState::OnUpdate() 
{
}
