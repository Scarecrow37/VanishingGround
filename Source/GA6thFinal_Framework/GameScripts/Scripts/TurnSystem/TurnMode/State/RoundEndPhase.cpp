#include "pchScripts.h"
#include "RoundEndPhase.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>

//Character
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, RoundEndPhase)

RoundEndPhase::RoundEndPhase() {}

RoundEndPhase::~RoundEndPhase() {}

void RoundEndPhase::OnAwake() {}

void RoundEndPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
    _isPhaseEnd = false;
}

void RoundEndPhase::OnEnter()
{
    NotifyRoundEnd();
    _isPhaseEnd = true;
}        

void RoundEndPhase::OnExit() 
{
    _isPhaseEnd = false;
}

void RoundEndPhase::OnUpdate() {}

void RoundEndPhase::NotifyRoundEnd()
{
    if (_turnMode)
    {
        auto* combatStartPhase = _turnMode->States->CombatStartPhase;
        if (combatStartPhase)
        {
            for (auto& character : combatStartPhase->GetCharacters())
            {
                character->OnRoundEnd();
            }
            _turnMode->ApplyActions([](TurnAction& action) { action.OnRoundEnd(); });
        }
    }
}
