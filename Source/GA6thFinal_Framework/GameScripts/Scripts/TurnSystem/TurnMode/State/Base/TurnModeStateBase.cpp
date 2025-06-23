#include "TurnModeStateBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"

TurnModeStateBase::TurnModeStateBase() {}

TurnModeStateBase::~TurnModeStateBase() {}

void TurnModeStateBase::OnStart() 
{
    _turnMode = GetFSM().GetComponent<TurnMode>();
}
