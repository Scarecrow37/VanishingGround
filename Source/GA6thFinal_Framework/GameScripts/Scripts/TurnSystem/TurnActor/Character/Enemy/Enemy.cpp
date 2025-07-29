#include "pchScripts.h"
#include "Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Enemy/EnemyStatsComponent.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnMode/TurnMode.h>

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
    if (auto turnMode = TurnMode::GetInstance())
    {
        turnMode->ApplyActions([this](TurnAction& action) { action.OnEnemyDead(*this); });
    }
}

void Enemy::TakeDamage(int damage) 
{
    // TODO: 피격 애니메이션 재생
    // 예외 사항 - 피격 애니메이션 재생 종료 후 원래 애니메이션으로 돌아가야함.


    // 혹시나 그럴 일 없겠지만 중간에 계산할 연산이 또 있다면 재연산
    int takeDamage = damage;
    Base::TakeDamage(takeDamage);
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

    InitMeshModel();
}

void Enemy::Update() 
{
}

CharacterStats* Enemy::GetCharacterStats()
{
    CharacterStats* stats = nullptr;
    EnemyStatsComponent* statsComponent = GetEnemyStats();
    if (nullptr != statsComponent)
    {
        stats = &statsComponent->GetStats();
    }
    return stats;
}

int Enemy::GetSpeed()
{
    int speed = 0;
    EnemyStatsComponent* stats = GetEnemyStats();
    if (nullptr != stats)
    {
        speed = stats->GetStats().Speed;
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

#define ANIM_NAME(enumType, name)\
case enumType :\
return name;\
break;
const char* Enemy::GetAnimationName(AnimationType type)
{
    EnemyType enemyType = Type;
    switch (enemyType)
    {
            // A
            case EnemyType::MONSTER_A:
            {
                switch (type)
                {
                    ANIM_NAME(IDLE, "")
                    ANIM_NAME(HIT, "")
                    ANIM_NAME(DEATH, "")
                    ANIM_NAME(ATTACK_1, "")
                    ANIM_NAME(ATTACK_2, "")
                    ANIM_NAME(ATTACK_3, "")
                    ANIM_NAME(ATTACK_4, "")
                    ANIM_NAME(ATTACK_READY, "")
                    ANIM_NAME(ATTACK_LOOP, "")
                    ANIM_NAME(ATTACK_END, "")
                default:
                    break;
                }
                break;
            }
            // B
            case EnemyType::MONSTER_B: 
            {
                switch (type)
                {
                    ANIM_NAME(IDLE,     "Armature|Enemy02_Anim_Idle01")
                    ANIM_NAME(HIT,      "Armature|Enemy02_Anim_GetHit")
                    ANIM_NAME(DEATH,    "Armature|Enemy02_Anim_Death")
                    ANIM_NAME(ATTACK_1, "Armature|Enemy02_Anim_Attack01")
                    ANIM_NAME(ATTACK_2, "")
                    ANIM_NAME(ATTACK_3, "Armature|Enemy02_Anim_Attack03")
                    ANIM_NAME(ATTACK_4, "")
                    ANIM_NAME(ATTACK_READY, "")
                    ANIM_NAME(ATTACK_LOOP, "")
                    ANIM_NAME(ATTACK_END, "")
                default:
                    break;
                }
                break;
            }
            // C
            case EnemyType::MONSTER_C: 
            {
                switch (type)
                {
                    ANIM_NAME(IDLE, "")
                    ANIM_NAME(HIT, "")
                    ANIM_NAME(DEATH, "")
                    ANIM_NAME(ATTACK_1, "")
                    ANIM_NAME(ATTACK_2, "")
                    ANIM_NAME(ATTACK_3, "")
                    ANIM_NAME(ATTACK_4, "")
                    ANIM_NAME(ATTACK_READY, "")
                    ANIM_NAME(ATTACK_LOOP, "")
                    ANIM_NAME(ATTACK_END, "")
                default:
                    break;
                }
                break;
            }
            default: 
            {
                break;
            }
           
    }
    return "";
}
#undef ANIM_NAME