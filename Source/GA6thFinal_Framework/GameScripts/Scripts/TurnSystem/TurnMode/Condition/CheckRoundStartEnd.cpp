#include "CheckRoundStartEnd.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnMode/State/RoundStartPhase.h"

REGISTER_CLASS(FSMConditionFactory, CheckRoundStartEnd)

CheckRoundStartEnd::CheckRoundStartEnd() 
{

}

CheckRoundStartEnd::~CheckRoundStartEnd() 
{

}

void CheckRoundStartEnd::OnAwake() 
{

}

void CheckRoundStartEnd::OnStart() 
{
    GetTurnModeBaseC::OnStart();
}

bool CheckRoundStartEnd::Evaluate()
{
    return _turnMode->States->RoundStartPhase->IsPhaseEnd();
}
