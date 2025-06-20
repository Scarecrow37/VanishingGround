#include "RoundEndPhase.h"

REGISTER_CLASS(FSMStateFactory, RoundEndPhase)

RoundEndPhase::RoundEndPhase() {}

RoundEndPhase::~RoundEndPhase() {}

void RoundEndPhase::OnAwake() {}

void RoundEndPhase::OnStart() 
{
    GetTurnModeBaseS::OnStart();
}

void RoundEndPhase::OnEnter() 
{
    _isPhaseEnd = false;

    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"라운드를 종료합니다.");
    UmTime.Invoke(&GetFSM(), 1.0f, [&]() 
    { 
        UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"다음 라운드까지 3.");
    });

    UmTime.Invoke(&GetFSM(), 2.0f, [&]() 
    { 
        UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"다음 라운드까지 2.");
    });

    UmTime.Invoke(&GetFSM(), 3.0f, [&]() 
    { 
        UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"다음 라운드까지 1.");
        _isPhaseEnd = true;
    });

}

void RoundEndPhase::OnExit() {}

void RoundEndPhase::OnUpdate() {}
