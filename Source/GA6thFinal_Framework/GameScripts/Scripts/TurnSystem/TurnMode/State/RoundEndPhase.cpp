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
    
}

void RoundEndPhase::OnExit() {}

void RoundEndPhase::OnUpdate() {}
