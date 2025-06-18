#include "RoundStartCondition.h"

REGISTER_CLASS(FSMConditionFactory, RoundStartCondition)

RoundStartCondition::RoundStartCondition() 
    : 
    _isRoundStart(false)
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

}

bool RoundStartCondition::Evaluate()
{
    return _isRoundStart;
}
