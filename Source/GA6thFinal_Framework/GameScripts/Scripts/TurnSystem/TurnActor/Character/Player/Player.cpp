#include "pchScripts.h"
#include "Player.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Player/PlayerStatsComponent.h"

Player::Player()
{

}
Player::~Player() = default;

void Player::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);

    if (nullptr == GetPlayerStats())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"Player Stats를 추가해주세요");
    }
}

void Player::Update() 
{
    bool isMyTurn = IsMyTurn;
    if (true == isMyTurn)
    {
        Vector3 delta = Vector3(0, 1080, 0) * Mathf::Deg2Rad * UmTime.DeltaTime();
        gameObject->transform->Rotation *= Quaternion::CreateFromYawPitchRoll(delta);
    }
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

void Player::ImGuiDrawPropertysEvent() 
{

}

CharacterStats* Player::GetCharacterStats()
{
    CharacterStats* stats = nullptr;
    PlayerStatsComponent* playerStatsComponent = GetPlayerStats();
    if (nullptr != playerStatsComponent)
    {
        stats = playerStatsComponent->GetStats();
    }
    return stats;
}

PlayerStatsComponent* Player::GetPlayerStats()
{
    if (nullptr == _playerStats)
    {
        _playerStats = GetComponent<PlayerStatsComponent>();
    }  
    return _playerStats;
}

int Player::GetManaRegenRate()
{
    int manaRegenRate = 0;
    PlayerStatsComponent* playerStats = GetPlayerStats();
    if (playerStats)
    {
        manaRegenRate = playerStats->GetStats()->ManaRegenRate;
    }
    return manaRegenRate;
}

int Player::GetShield()
{
    int shield = 0;
    PlayerStatsComponent* playerStats = GetPlayerStats();
    if (playerStats)
    {
        shield = playerStats->GetStats()->Shield;
    }
    return shield;
}

int Player::GetSpeed()
{
    return 0;
}
