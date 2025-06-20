#include "CheckPlayerCondition.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REGISTER_CLASS(FSMConditionFactory, CheckPlayerCondition)

CheckPlayerCondition::CheckPlayerCondition() 
{

}

CheckPlayerCondition::~CheckPlayerCondition() 
{

}

void CheckPlayerCondition::OnAwake() 
{

}

void CheckPlayerCondition::OnStart() 
{
    GetTurnModeBaseC::OnStart();
}

bool CheckPlayerCondition::Evaluate()
{
    return 0 < _turnMode->GetPendingActorCount();
}
