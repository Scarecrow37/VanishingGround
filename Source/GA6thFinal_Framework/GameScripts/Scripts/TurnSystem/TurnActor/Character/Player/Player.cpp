#include "pchScripts.h"
#include "Player.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include <WeaponSystem/WeaponTable/WeaponTableComponent.h>
#include <WeaponSystem/WeaponSystem.h>

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

Player::~Player()
{
    if (this == static_instance)
    {
        static_instance = nullptr;
    }
}

void Player::Awake() 
{
    if (nullptr == static_instance)
    {
        static_instance = this;
    }
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

void Player::SerializedReflectEvent() 
{
   
}

void Player::DeserializedReflectEvent() 
{

}

int Player::GetSpeed()
{
    WeaponSystem* system = WeaponSystem::GetInstance();
    if (system)
    {
        return system->GetCurrentWeaponStats().Speed;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8" WeaponSystem이 존재하지 않습니다.");
        return 0;
    }   
}

int Player::GetRandomSpeed()
{

    WeaponSystem* system = WeaponSystem::GetInstance();
    if (system)
    {
        return system->GetCurrentWeaponStats().RandomSpeed;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8" WeaponSystem이 존재하지 않습니다.");
        return 0;
    }   
}

void Player::PlayTurn()
{
    Base::PlayTurn();
    WeaponSystem* system = WeaponSystem::GetInstance();
    if (system)
    {
        std::string_view weaponName = system->GetCurrentWeaponStats().Name;
        std::string      message    = std::format("{}{}{}", (const char*)u8"Player 턴 시작. ", "Weapon : ", weaponName);
        UmLogger.Message(LogLevel::LEVEL_TRACE, message);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8" WeaponSystem이 존재하지 않습니다.");
    }     
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

void Player::TakeDamage(int damage)
{  
    // TODO: 피격 애니메이션 재생
    // 예외 사항 - 피격 애니메이션 재생 종료 후 원래 애니메이션으로 돌아가야함.

    // 혹시나 그럴 일 없겠지만 중간에 계산할 연산이 또 있다면 재연산
    int takeDamage = damage;
    Base::TakeDamage(takeDamage);
}


void Player::ImGuiDrawPropertysEvent()
{
    Base::ImGuiDrawPropertysEvent();
}

CharacterStats* Player::GetCharacterStats()
{
    CharacterStats* stats = nullptr;
    PlayerStatsComponent* playerStatsComponent = GetPlayerStats();
    if (nullptr != playerStatsComponent)
    {
        stats = &playerStatsComponent->GetStats();
    }
    return stats;
}


PlayerStatsComponent* Player::GetPlayerStats()
{
    if (nullptr == _playerStats)
    {
        _playerStats = GetComponent<PlayerStatsComponent>();
        if (nullptr == _playerStats)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"플레이어 스텟이 존재하지 않습니다.");
        }
    }  
    return _playerStats;
}

int Player::GetShield()
{
    int shield = 0;
    PlayerStatsComponent* playerStats = GetPlayerStats();
    if (playerStats)
    {
        shield = playerStats->GetStats().Shield;
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

void Player::OnCombatStart()
{
    Base::OnCombatStart();
}

void Player::OnRoundStart()
{
    Base::OnRoundStart();
}

void Player::OnRoundEnd()
{
    Base::OnRoundEnd();
}

void Player::OnEachTurnStart(CharacterBase* destination) 
{
    Base::OnEachTurnStart(destination);
}

void Player::OnTurnStart()
{
    Base::OnTurnStart();
}

void Player::OnTurnEnd()
{
    Base::OnTurnEnd();
}

void Player::OnHit()
{
    Base::OnHit();
}

void Player::OnDead()
{
    Base::OnDead();
}

void Player::OnKill(CharacterBase* destination)
{
    Base::OnKill(destination);
}

void Player::OnTokenAdded(int tokenID)
{
    Base::OnTokenAdded(tokenID);
}

void Player::OnTokenRemoved(int tokenID)
{
    Base::OnTokenRemoved(tokenID);
}

#define ANIM_NAME(enumType, name)\
case enumType :\
return name;\
break;
const char* Player::GetAnimationName(AnimationType type)
{
    switch (type)
    {
        ANIM_NAME(IDLE,         "rig|Player_Anim_Idle")
        ANIM_NAME(HIT,          "rig|Player_Anim_GetHit")
        ANIM_NAME(ATTACK_READY, "rig|Player_Anim_Attack_Ready")
        ANIM_NAME(ATTACK_LOOP,  "rig|Player_Anim_Attack")
        ANIM_NAME(ATTACK_END,   "rig|Player_Anim_Attack_End")
        ANIM_NAME(DEATH,        "rig|Player_Anim_Death")

        default:
        {
            return "";
            break;
        }
    }
}
