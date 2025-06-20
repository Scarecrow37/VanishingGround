#include "CheckRoundStartExit.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnMode/State/RoundStartPhase.h"

REGISTER_CLASS(FSMConditionFactory, CheckRoundStartExit)

CheckRoundStartExit::CheckRoundStartExit() 
{

}

CheckRoundStartExit::~CheckRoundStartExit() 
{

}

void CheckRoundStartExit::OnAwake() 
{

}

void CheckRoundStartExit::OnStart() 
{
    GetTurnModeBaseC::OnStart();
}

bool CheckRoundStartExit::Evaluate()
{
    return _turnMode->States->RoundStartPhase->IsPhaseEnd();
}
