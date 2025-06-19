#include "Player.h"
Player::Player()
{
    MaxHP      = 100;
    MaxMP      = 100;
    ChainCount = 0;
}
Player::~Player() = default;

void Player::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
}

int Player::GetSpeed()
{
    return 0;
}
