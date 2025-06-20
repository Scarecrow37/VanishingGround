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
    UmTime.Invoke(this, 1.f, []() { UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 종료 3"); });
    UmTime.Invoke(this, 2.f, []() { UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 종료 2"); });
    UmTime.Invoke(this, 3.f, [this]() 
    { 
        UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 종료 1"); 
    });
    UmTime.Invoke(this, 4.f, [this]() { this->MyTurnEnd(); });
}

void Player::OnTurnEnd() 
{
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 종료.");
}

int Player::GetSpeed()
{
    return 0;
}
