#include "Enemy.h"

Enemy::Enemy()
{
    MaxHP      = 100;
    MaxMP      = 100;
    ChainCount = 0;
    Speed      = 0;
}

Enemy::~Enemy() = default;

void Enemy::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
}

void Enemy::OnTurnStart() 
{
    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 시작.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
}

int Enemy::GetSpeed()
{
    return Speed;
}
