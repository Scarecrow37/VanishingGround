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
    if (auto& currActorModel = _turnMode->GetCurrTurnActor())
    {
        return TurnActor::STATE::Play != currActorModel->State;
    }
    return false;
}
