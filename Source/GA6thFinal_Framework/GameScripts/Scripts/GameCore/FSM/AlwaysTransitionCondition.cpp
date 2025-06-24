#include "pchScripts.h"
#include "AlwaysTransitionCondition.h"

REGISTER_CLASS(FSMConditionFactory, AlwaysTransitionCondition)

void AlwaysTransitionCondition::OnAwake() {}

void AlwaysTransitionCondition::OnStart() {}

bool AlwaysTransitionCondition::Evaluate()
{
    return true;
}
