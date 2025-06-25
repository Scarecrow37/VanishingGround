#include "pchScripts.h"
#include "CheckTurnNotEmpty.h"

REGISTER_CLASS(FSMConditionFactory, CheckTurnNotEmpty)

bool CheckTurnNotEmpty::Evaluate()
{
    return !CheckTurnEmpty::Evaluate();
}
