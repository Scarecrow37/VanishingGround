#include "TurnMode.h"
#include "GameCore/FSM/FiniteStateMachine.h"

TurnMode::TurnMode() = default;
TurnMode::~TurnMode() = default;

void TurnMode::BuildTurnModeFSM() 
{
    _finiteStateMachine = GetComponent<FiniteStateMachine>();
    if (nullptr == _finiteStateMachine)
    {
        _finiteStateMachine = &AddComponent<FiniteStateMachine>();

    }
}

void TurnMode::Reset() 
{
   
}

void TurnMode::Awake() 
{
    BuildTurnModeFSM();
}

void TurnMode::Start() 
{

}

void TurnMode::Update() 
{

}
