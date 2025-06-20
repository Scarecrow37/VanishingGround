#include "GetTurnModeBaseC.h"
#include "TurnSystem/TurnMode/TurnMode.h"

GetTurnModeBaseC::GetTurnModeBaseC() 
{

}

GetTurnModeBaseC::~GetTurnModeBaseC() 
{

}

void GetTurnModeBaseC::OnStart() 
{
    _turnMode = GetFSM().GetComponent<TurnMode>();
}
