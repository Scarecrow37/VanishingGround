#include "pchScripts.h"
#include "CheckRoundEndExit.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnMode/State/RoundEndPhase.h"

REGISTER_CLASS(FSMConditionFactory, CheckRoundEndExit)

CheckRoundEndExit::CheckRoundEndExit() {}

CheckRoundEndExit::~CheckRoundEndExit() {}

void CheckRoundEndExit::OnAwake() {}

void CheckRoundEndExit::OnStart() 
{
    TurnModeConditionBase::OnStart();
}

bool CheckRoundEndExit::Evaluate()
{
    return _turnMode->States->RoundEndPhase->IsPhaseEnd();
}
