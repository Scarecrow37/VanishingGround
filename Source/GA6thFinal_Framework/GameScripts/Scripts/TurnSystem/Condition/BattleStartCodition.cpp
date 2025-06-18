#include "BattleStartCodition.h"

BattleStartCodition::BattleStartCodition() 
    : 
    _isBattleStart(false) 
{
}

BattleStartCodition::~BattleStartCodition() 
{
}

void BattleStartCodition::OnAwake() {}

void BattleStartCodition::OnStart() {}

bool BattleStartCodition::Evaluate()
{
    return _isBattleStart;
}
