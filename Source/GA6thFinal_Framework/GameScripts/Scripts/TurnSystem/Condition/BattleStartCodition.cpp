#include "BattleStartCodition.h"
#include "../State/BattleStartPhase.h"

REGISTER_CLASS(FSMConditionFactory, BattleStartCodition)

BattleStartCodition::BattleStartCodition() 
    :
    _battleStartPhase(nullptr)
{

}

BattleStartCodition::~BattleStartCodition() 
{
}

void BattleStartCodition::OnAwake() {}

void BattleStartCodition::OnStart() 
{
    _battleStartPhase = GetFSM().GetState<BattleStartPhase>();
}

bool BattleStartCodition::Evaluate()
{
    return _battleStartPhase->IsEndPhase();
}
