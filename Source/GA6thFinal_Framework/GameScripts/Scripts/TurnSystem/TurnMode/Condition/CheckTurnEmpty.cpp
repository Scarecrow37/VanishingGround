#include "pchScripts.h"
#include "CheckTurnEmpty.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REGISTER_CLASS(FSMConditionFactory, CheckTurnEmpty)

CheckTurnEmpty::CheckTurnEmpty() {}

CheckTurnEmpty::~CheckTurnEmpty() {}

void CheckTurnEmpty::OnAwake() 
{
   
}

void CheckTurnEmpty::OnStart() 
{
    TurnModeConditionBase::OnStart();
}

bool CheckTurnEmpty::Evaluate()
{
    return 0 == _turnMode->GetPendingActorCount();
}
