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
#include "State/PlayerWaitTurnState.h"
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

int Player::GetSpeed()
{
    return 0;
}

void Player::PlayTurn()
{
    Base::PlayTurn();
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 시작");
}

void Player::EndTurn()
{
    Base::EndTurn();
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 종료.");
}

void Player::Dead()
{
    Base::Dead();
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
        _fsmStates.PlayerWaitTurnState = _finiteStateMachine->AddState<PlayerWaitTurnState>();
        _fsmStates.PlayerPlayTurnState = _finiteStateMachine->AddState<PlayerPlayTurnState>();
        _fsmStates.PlayerDeadState     = _finiteStateMachine->AddState<PlayerDeadState>();

        //Transition
        _finiteStateMachine->AddTransition<PlayerWaitTurnState, PlayerStartCondition, PlayerPlayTurnState>();
        _finiteStateMachine->AddTransition<PlayerPlayTurnState, PlayerExitCondition, PlayerWaitTurnState>();

        _finiteStateMachine->AddTransition<PlayerDeadCondition, PlayerDeadState>();
        _finiteStateMachine->AddTransition<PlayerDeadState, PlayerExitCondition, PlayerWaitTurnState>();

        //Entry
        _finiteStateMachine->SetEntryState<PlayerWaitTurnState>();
    }
}

void Player::OnInput(Input::Controller* controller) 
{
    float dt = UmTime.DeltaTime();
    using ThumbStickAxis = Input::Controller::ThumbStickAxis;
    ThumbStickAxis leftAxis = controller->GetLeftThumbStickAxis();
    Vector3 dir(leftAxis.X, 0.f, leftAxis.Y);
    transform->Position += dir * _moveSpeed * leftAxis.Magnitude;

    ThumbStickAxis rightAxis = controller->GetRightThumbStickAxis();
    if (rightAxis.Magnitude > 0)
    {
        transform->Rotate(Vector3(rightAxis.Y, rightAxis.X, 0), dt * _rotSpeed * rightAxis.Magnitude);
    }

    float lt = controller->GetLeftTrigger();
    if (lt > 0)
    {
        UmLogger.Log(LogLevel::LEVEL_TRACE, std::format("Lt : {}", lt));
    }
    float rt = controller->GetRightTrigger();
    if (rt > 0)
    {
        UmLogger.Log(LogLevel::LEVEL_TRACE, std::format("Lt : {}", rt));
    }
                  
    const std::pair<const char*, Input::Controller::Button> ButtonList[] = 
    {
        {"DPAD_UP", Input::Controller::DPAD_UP},
        {"DPAD_DOWN", Input::Controller::DPAD_DOWN},
        {"DPAD_LEFT", Input::Controller::DPAD_LEFT},
        {"DPAD_RIGHT", Input::Controller::DPAD_RIGHT},
        {"START", Input::Controller::START},
        {"BACK", Input::Controller::BACK},
        {"LEFT_THUMB", Input::Controller::LEFT_THUMB},
        {"RIGHT_THUMB", Input::Controller::RIGHT_THUMB},
        {"LEFT_SHOULDER", Input::Controller::LEFT_SHOULDER},
        {"RIGHT_SHOULDER", Input::Controller::RIGHT_SHOULDER},
        {"A", Input::Controller::A},
        {"B", Input::Controller::B},
        {"X", Input::Controller::X},
        {"Y", Input::Controller::Y},
    };

    for (const auto& [name, button] : ButtonList)
    {
        if (controller->IsButtonDown(button))
        {
            UmLogger.Log(LogLevel::LEVEL_TRACE, name);
        }
    }
}