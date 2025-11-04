#include "pchScripts.h" 
#include "EnemyActionPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/TurnActor.h"

#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnMode/Condition/CheckTurnEndCondition.h>

REGISTER_CLASS(FSMStateFactory, EnemyActionPhase)

EnemyActionPhase::EnemyActionPhase() {}

EnemyActionPhase::~EnemyActionPhase() {}

void EnemyActionPhase::OnAwake() {}

void EnemyActionPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void EnemyActionPhase::OnEnter() 
{
    if (_turnMode)
    {
        WaitPhase = true;
        if (auto& actorModel = _turnMode->GetCurrTurnActor())
        {
            if (TurnActor* actor = actorModel.Get())
            {
                actor->PlayTurn();
                actor->OnTurnStart();
                CombatStartPhase* combatStartPhase = _turnMode->States->CombatStartPhase;
                if (combatStartPhase)
                {
                    CharacterBase* character = static_cast<CharacterBase*>(actor);
                    for (auto& ch : combatStartPhase->GetCharacters())
                    {
                        ch->OnEachTurnStart(character);
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
}

void EnemyActionPhase::OnExit() 
{
    WaitPhase = true;

    if (_turnMode)
    {
        if (auto& actorModel = _turnMode->GetCurrTurnActor())
        {
            if (TurnActor* actor = actorModel.Get())
            {
                actor->OnTurnEnd();
            }
        }
    }
}

void EnemyActionPhase::OnUpdate() 
{
    if (_turnMode && false == WaitPhase)
    {
        const auto& currentModel =_turnMode->GetCurrTurnActor();
        if (CheckTurnEndCondition* condition = _turnMode->Conditions->CheckTurnEndCondition)
        {
            condition->IsTurnEnd = TurnActor::STATE::Play != currentModel->GetActorState();
        }
    }
}
