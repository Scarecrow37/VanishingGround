#include "pchScripts.h"
#include "PlayerActionPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/TurnActor.h"

#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnMode/Condition/CheckTurnEndCondition.h>

REGISTER_CLASS(FSMStateFactory, PlayerActionPhase)

PlayerActionPhase::PlayerActionPhase() 
{
}

PlayerActionPhase::~PlayerActionPhase() 
{
}

void PlayerActionPhase::OnAwake() 
{
}

void PlayerActionPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void PlayerActionPhase::OnEnter() 
{
    if (_turnMode)
    {
        if (auto& actor = _turnMode->GetCurrTurnActor())
        {
            actor->PlayTurn();
            if (auto* combatStartPhase = _turnMode->States->CombatStartPhase)
            {
                CharacterBase* character = static_cast<CharacterBase*>(actor.Get());
                for (auto& cha : combatStartPhase->GetCharacters())
                {
                    cha->OnEachTurnStart(character);
                }
                _turnMode->ApplyActions([character](TurnAction& action) { action.OnTurnStart(*character); });
            }
            actor->UpdatePostTurnState();
        }
    }
    if (CheckTurnEndCondition* condition = _turnMode->Conditions->CheckTurnEndCondition)
    {
        condition->IsTurnEnd = false;
        // 토큰 데미지를 기다린다
        if (TokenSystem* system = SingletonComponent<TokenSystem>::GetInstance())
        {
            float tokenDelayTime = system->TokenDamageDelayTime * 2; // 두개의 토큰 대미지를 기다려야함 (출혈, 중독)
            UmTime.Invoke(GetFSM(), tokenDelayTime, [this]() 
            { 
                WaitPhase = false; 
                UpdateCharacterDead();
            });
        }
        else
        {
            WaitPhase = false;
            UpdateCharacterDead();
        }
    }
}

void PlayerActionPhase::OnExit()
{
    WaitPhase = true;
    ApplyReduceHP();
}

void PlayerActionPhase::OnUpdate() 
{
    if (_turnMode && false == WaitPhase)
    {
        const auto& currentModel = _turnMode->GetCurrTurnActor();
        if (CheckTurnEndCondition* condition = _turnMode->Conditions->CheckTurnEndCondition)
        {
            condition->IsTurnEnd = TurnActor::STATE::Play != currentModel->GetActorState();
        }
    }
}
