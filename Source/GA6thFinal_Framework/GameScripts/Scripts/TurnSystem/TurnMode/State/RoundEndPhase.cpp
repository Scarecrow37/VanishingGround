#include "pchScripts.h"
#include "RoundEndPhase.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>

//Character
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, RoundEndPhase)

RoundEndPhase::RoundEndPhase() {}

RoundEndPhase::~RoundEndPhase() {}

void RoundEndPhase::OnAwake() {}

void RoundEndPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void RoundEndPhase::OnEnter()
{
    _isPhaseEnd = false;

    // [Callback] OnRoundEnd
    NotifyRoundEnd();

    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"라운드를 종료합니다!!!!===========================================================");
    UmTime.Invoke(&GetFSM(), 1.0f, [&]() { UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"다음 라운드까지 3."); });
    UmTime.Invoke(&GetFSM(), 2.0f, [&]() { UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"다음 라운드까지 2."); });
    UmTime.Invoke(&GetFSM(), 3.0f, [&]() { UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"다음 라운드까지 1."); });
    UmTime.Invoke(&GetFSM(), 4.0f, [&]() { _isPhaseEnd = true; });
}        


void RoundEndPhase::OnExit() {}

void RoundEndPhase::OnUpdate() {}

void RoundEndPhase::NotifyRoundEnd()
{
    if (_turnMode)
    {
        auto* combatStartPhase = _turnMode->States->CombatStartPhase;
        if (combatStartPhase)
        {
            Player* player = combatStartPhase->GetPlayer();
            if (player)
            {
                player->OnRoundEnd();
            }
            const auto& enemies = combatStartPhase->GetEnemies();
            for (const auto& enemy : enemies)
            {
                if (enemy)
                {
                    enemy->OnRoundEnd();
                }
            }
        }
    }
}
