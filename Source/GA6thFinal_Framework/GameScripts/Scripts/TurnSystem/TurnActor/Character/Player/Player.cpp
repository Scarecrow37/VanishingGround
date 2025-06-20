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

void Player::OnTurnStart() 
{
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 시작");
}

int Player::GetSpeed()
{
    return 0;
}
