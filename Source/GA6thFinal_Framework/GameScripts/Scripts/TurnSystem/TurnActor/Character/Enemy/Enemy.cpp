#include "pchScripts.h"
#include "Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Enemy/EnemyStatsComponent.h"
#include <GameCore/FSM/FiniteStateMachine.h>

Enemy::Enemy()
{

}

Enemy::~Enemy() = default;

void Enemy::EndTurnEnemy() {}

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

void Enemy::OnTurnStart() 
{
    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 시작.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 3.");
    UmTime.Invoke(this, 1.f, [=]() { UmLogger.Message(LogLevel::LEVEL_TRACE, message); });

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 2.");
    UmTime.Invoke(this, 2.f, [=]() { UmLogger.Message(LogLevel::LEVEL_TRACE, message); });

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 1.");
    UmTime.Invoke(this, 3.f, [=]() 
    {
        UmLogger.Message(LogLevel::LEVEL_TRACE, message);
    });

    UmTime.Invoke(this, 4.f, [=](){ this->EndTurn(); });
}

void Enemy::OnTurnEnd() 
{
    static std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
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

        //State
    }
}
