#include "pchScripts.h"
#include "Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Enemy/EnemyStatsComponent.h"
#include <GameCore/FSM/FiniteStateMachine.h>

//Condition
#include "Condition/EnemyStartCondition.h"
#include "Condition/EnemyEndCondition.h"
#include "Condition/EnemyDeadCondition.h"

//State
#include "State/EnemyWaitTurnState.h"
#include "State/EnemyPlayTurnState.h"
#include "State/EnemyDeadState.h"

Enemy::Enemy()
{

}

Enemy::~Enemy() = default;

void Enemy::PlayTurn() 
{
    Base::PlayTurn();

}

void Enemy::EndTurn() 
{
    Base::EndTurn();
}

void Enemy::Revive() 
{
    Base::Revive();
}

void Enemy::Dead()
{
    Base::Dead();
}

void Enemy::Awake()
{
    Base::Awake();
    gameObject->AddTag(TAG);
    BuildEnemyFSM();

    if (nullptr == GetEnemyStats())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"Enemy Stats를 추가해주세요");
    }
}

void Enemy::Update() 
{
    bool isMyTurn = IsMyTurn;
    if (isMyTurn)
    {
        Vector3 delta = Vector3(0, 1080, 0) * Mathf::Deg2Rad * UmTime.DeltaTime();
        gameObject->transform->Rotation *= Quaternion::CreateFromYawPitchRoll(delta);
    }
}

CharacterStats* Enemy::GetCharacterStats()
{
    CharacterStats* stats = nullptr;
    EnemyStatsComponent* statsComponent = GetEnemyStats();
    if (nullptr != statsComponent)
    {
        stats = statsComponent->GetStats();
    }
    return stats;
}

int Enemy::GetSpeed()
{
    int speed = 0;
    EnemyStatsComponent* stats = GetEnemyStats();
    if (nullptr != stats)
    {
        speed = stats->GetStats()->Speed;
    }
    return speed;
}

EnemyStatsComponent* Enemy::GetEnemyStats()
{
    if (nullptr == _enemyStats)
    {
        _enemyStats = GetComponent<EnemyStatsComponent>();
        if (nullptr == _enemyStats)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Enemy Stats 컴포넌트가 존재하지 않습니다.");
        }
    }
    return _enemyStats;
}

void Enemy::BuildEnemyFSM() 
{
    _finiteStateMachine = GetComponent<FiniteStateMachine>();
    if (nullptr == _finiteStateMachine)
    {
        _finiteStateMachine = &AddComponent<FiniteStateMachine>();

        //Condition
        _finiteStateMachine->AddCondition<EnemyStartCondition>();
        _finiteStateMachine->AddCondition<EnemyEndCondition>();
        _finiteStateMachine->AddCondition<EnemyDeadCondition>();

        //State
        _fsmStates.WaitTurn = _finiteStateMachine->AddState<EnemyWaitTurnState>();
        _fsmStates.PlayTurn = _finiteStateMachine->AddState<EnemyPlayTurnState>();
        _fsmStates.Dead     = _finiteStateMachine->AddState<EnemyDeadState>();

        //Transition
        _finiteStateMachine->AddTransition<EnemyWaitTurnState, EnemyStartCondition, EnemyPlayTurnState>();
        _finiteStateMachine->AddTransition<EnemyPlayTurnState, EnemyEndCondition, EnemyWaitTurnState>();
        _finiteStateMachine->AddTransition<EnemyDeadCondition, EnemyDeadState>();

        //Entry
        _finiteStateMachine->SetEntryState<EnemyWaitTurnState>();
    }
}

void Enemy::OnCombatStart()
{
    Base::OnCombatStart();
}

void Enemy::OnRoundStart()
{
    Base::OnRoundStart();
}

void Enemy::OnRoundEnd()
{
    Base::OnRoundEnd();
}

void Enemy::OnEachTurnStart(CharacterBase* destination)
{
    Base::OnEachTurnStart(destination);
}

void Enemy::OnTurnStart()
{
    Base::OnTurnStart();
}

void Enemy::OnTurnEnd()
{
    Base::OnTurnEnd();
}

void Enemy::OnHit()
{
    Base::OnHit();
}

void Enemy::OnDead()
{
    Base::OnDead();
}

void Enemy::OnKill(CharacterBase* destination)
{
    Base::OnKill(destination);
}

void Enemy::OnTokenAdded(int tokenID)
{
    Base::OnTokenAdded(tokenID);
}

void Enemy::OnTokenRemoved(int tokenID)
{
    Base::OnTokenRemoved(tokenID);
}