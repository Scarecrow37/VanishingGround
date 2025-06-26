#include "pchScripts.h"
#include "Player.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "GameCore/FSM/FiniteStateMachine.h"

//Condition
#include "Condition/PlayerStartCondition.h"
#include "Condition/PlayerExitCondition.h"
#include "Condition/PlayerDeadCondition.h"

//State
#include "State/PlayerTurnWaitState.h"
#include "State/PlayerPlayTurnState.h"
#include "State/PlayerDeadState.h"

Player::Player()
{

}
Player::~Player() = default;

void Player::Awake() 
{
    Base::Awake();
    gameObject->AddTag(TAG);
    BuildPlayerFSM();

    if (nullptr == GetPlayerStats())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"Player Stats를 추가해주세요");
    }
}

void Player::Update() 
{

}

void Player::OnTurnStart()
{
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 시작");
}

void Player::OnTurnEnd() 
{
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 종료.");
}

void Player::OnRevive() 
{
    Base::OnRevive();
}

void Player::OnDead() 
{
    Base::OnDead();
    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"플레이어 사망!!!");
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

void Player::BuildPlayerFSM() 
{
    _finiteStateMachine = GetComponent<FiniteStateMachine>();
    if (nullptr == _finiteStateMachine)
    {
        _finiteStateMachine = &AddComponent<FiniteStateMachine>();

        //Conditions
        _finiteStateMachine->AddCondition<PlayerStartCondition>();
        _finiteStateMachine->AddCondition<PlayerExitCondition>();
        _finiteStateMachine->AddCondition<PlayerDeadCondition>();

        //States
        _fsmStates.PlayerTurnWaitState = _finiteStateMachine->AddState<PlayerTurnWaitState>();
        _fsmStates.PlayerPlayTurnState = _finiteStateMachine->AddState<PlayerPlayTurnState>();
        _fsmStates.PlayerDeadState     = _finiteStateMachine->AddState<PlayerDeadState>();

        //Transition
        _finiteStateMachine->AddTransition<PlayerTurnWaitState, PlayerStartCondition, PlayerPlayTurnState>();
        _finiteStateMachine->AddTransition<PlayerPlayTurnState, PlayerExitCondition, PlayerTurnWaitState>();

        _finiteStateMachine->AddTransition<PlayerDeadCondition, PlayerDeadState>();
        _finiteStateMachine->AddTransition<PlayerDeadState, PlayerExitCondition, PlayerTurnWaitState>();

        //Entry
        _finiteStateMachine->SetEntryState<PlayerTurnWaitState>();
    }
}

void Player::EndTurn()
{
    Base::EndTurn();
    EndTurn();
}

void Player::Dead()
{
    Base::Dead();
}

int Player::GetSpeed()
{
    return 0;
}
