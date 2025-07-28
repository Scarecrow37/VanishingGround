#include "pchScripts.h"
#include "CheckTurnEndCondition.h"
#include "TurnSystem/TurnActor/TurnActor.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REGISTER_CLASS(FSMConditionFactory, CheckTurnEndCondition)

CheckTurnEndCondition::CheckTurnEndCondition() {}

CheckTurnEndCondition::~CheckTurnEndCondition() {}

void CheckTurnEndCondition::OnAwake() {}

void CheckTurnEndCondition::OnStart() 
{
    TurnModeConditionBase::OnStart();
}

bool CheckTurnEndCondition::Evaluate()
{
    TurnActor* currActor = _turnMode->GetCurrTurnActor();
    if (currActor)
    {
        bool isTurnEnd = TurnActor::STATE::Play != currActor->State;
        if (isTurnEnd)
        {
            currActor->OnTurnEnd();
        }
        return isTurnEnd;
    }
    return false;
}
