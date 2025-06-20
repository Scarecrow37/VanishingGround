#include "CombatStartCodition.h"

REGISTER_CLASS(FSMConditionFactory, CombatStartCodition)

CombatStartCodition::CombatStartCodition() 
{

}

CombatStartCodition::~CombatStartCodition() 
{
}

void CombatStartCodition::OnAwake() {}

void CombatStartCodition::OnStart() 
{
    
}

bool CombatStartCodition::Evaluate()
{
    return true;
}
