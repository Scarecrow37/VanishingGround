#include "RoundStartCondition.h"
#include "../State/CombatStartPhase.h"

REGISTER_CLASS(FSMConditionFactory, RoundStartCondition)

RoundStartCondition::RoundStartCondition() 
    :
    _combatStartPhase(nullptr) 
{

}

RoundStartCondition::~RoundStartCondition() 
{

}

void RoundStartCondition::OnAwake() 
{

}

void RoundStartCondition::OnStart() 
{
    _combatStartPhase = GetFSM().GetState<CombatStartPhase>();
}

bool RoundStartCondition::Evaluate()
{
    return _combatStartPhase->IsEndPhase();
}
