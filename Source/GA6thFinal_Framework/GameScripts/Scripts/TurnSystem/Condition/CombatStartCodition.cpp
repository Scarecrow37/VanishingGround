#include "CombatStartCodition.h"
#include "../State/CombatStartPhase.h"

REGISTER_CLASS(FSMConditionFactory, CombatStartCodition)

CombatStartCodition::CombatStartCodition() 
    :
    _battleStartPhase(nullptr)
{

}

CombatStartCodition::~CombatStartCodition() 
{
}

void CombatStartCodition::OnAwake() {}

void CombatStartCodition::OnStart() 
{
    _battleStartPhase = GetFSM().GetState<CombatStartPhase>();
}

bool CombatStartCodition::Evaluate()
{
    return _battleStartPhase->IsEndPhase();
}
