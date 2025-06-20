#include "GetTurnModeBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"

GetTurnModeBase::GetTurnModeBase() {}

GetTurnModeBase::~GetTurnModeBase() {}

void GetTurnModeBase::OnStart() 
{
    _turnMode = GetFSM().GetComponent<TurnMode>();
}
