#include "GetTurnModeBaseS.h"
#include "TurnSystem/TurnMode/TurnMode.h"

GetTurnModeBaseS::GetTurnModeBaseS() {}

GetTurnModeBaseS::~GetTurnModeBaseS() {}

void GetTurnModeBaseS::OnStart() 
{
    _turnMode = GetFSM().GetComponent<TurnMode>();
}
