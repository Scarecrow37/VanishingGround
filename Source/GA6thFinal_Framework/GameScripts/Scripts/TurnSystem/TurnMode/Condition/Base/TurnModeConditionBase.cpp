#include "pchScripts.h"
#include "TurnModeConditionBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"

TurnModeConditionBase::TurnModeConditionBase() 
{

}

TurnModeConditionBase::~TurnModeConditionBase() 
{

}

void TurnModeConditionBase::OnStart() 
{
    _turnMode = GetFSM().GetComponent<TurnMode>();
}
