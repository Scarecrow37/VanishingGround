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

int Enemy::GetSpeed()
{
    return Speed;
}
