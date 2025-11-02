#include "pchScripts.h"
#include "Player.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Player/PlayerStatsComponent.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include <WeaponSystem/WeaponTable/WeaponTableComponent.h>
#include <WeaponSystem/WeaponSystem.h>
#include <Mesh/SkeletalMeshRenderer.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <Particle/ParticleComponent.h>
#include <PlayerSystem/PlayerSystem.h>

//Condition
#include "Condition/PlayerStartCondition.h"
#include "Condition/PlayerExitCondition.h"
#include "Condition/PlayerDeadCondition.h"
#include "Condition/PlayerWinCondition.h"   

//State
#include "CombatUIManager/CombatUIManager.h"
#include "State/PlayerWaitTurnState.h"
#include "State/PlayerPlayTurnState.h"
#include "State/PlayerDeadState.h"
#include "State/PlayerWinState.h"
#include "UI/Contents/SpawnDamagePanel.h"

UMREAL_COMPONENT(Player)

Player::Player()
{
}

Player::~Player()
{
 
}

void Player::Awake() 
{
    if (_singletonComponent.TrySingleTon())
    {
        Base::Awake();
        gameObject->AddTag(TAG);
        BuildPlayerFSM();
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
    WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance();
    if (system)
    {
        return system->GetCurrentWeaponElement().Stats.Speed;
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8" WeaponSystem이 존재하지 않습니다.");
        return 0;
    }   
}

int Player::GetRandomSpeed()
{

    WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance();
    if (system)
    {
        return system->GetCurrentWeaponElement().Stats.RandomSpeed;
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
}

void Player::EndTurn()
{
    Base::EndTurn();
}

void Player::Dead()
{
    Base::Dead();
    if (auto turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        turnMode->ApplyActions([this](TurnAction& action) { action.OnPlayerDead(*this); });
    }
}

void Player::TakeDamage(int damage, const bool playAnim) 
{
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    if (turnMode)
    {
        turnMode->ApplyActions([&](TurnAction& action) { action.OnPlayerTakeDamageStart(*this, damage); });
    }
    int takeDamage = damage;
    Base::TakeDamage(takeDamage, playAnim);
    ShowDamage(damage, {});
    if (turnMode)
    {
        turnMode->ApplyActions([&](TurnAction& action) { action.OnPlayerTakeDamageEnd(*this, damage); });
    }

    if (ParticleComponent* particle = GetParticleComponent())
    {
        particle->PlayEffect("gethit");
    }
}

void Player::ShowDamage(const int damage, const std::span<const std::string> sources)
{
    if (const CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        [[maybe_unused]] auto _ = combatUI->CharacterHUDGroup.PlayerSpawnDamagePanel->MakeDamage(damage, sources);
    }
}

void Player::Heal(const int amount)
{
    Base::Heal(amount);
    ShowHeal(amount, {});
}

void Player::ShowHeal(const int healAmount, const std::span<const std::string> sources)
{
    if (const CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        [[maybe_unused]] auto _ = combatUI->CharacterHUDGroup.PlayerSpawnHealPanel->MakeDamage(healAmount, sources);
    }
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
        GameObject* playerSystem = SingletonObject<PlayerSystem>::GetInstance();
        if (playerSystem)
        {
            _playerStats = playerSystem->GetComponent<PlayerStatsComponent>();
        }
    }  
    if (nullptr == _playerStats)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"플레이어 시스템에 스텟이 존재하지 않습니다.");
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
        _finiteStateMachine->AddCondition<PlayerWinCondition>();

        //States
        _fsmStates.PlayerWaitTurnState = _finiteStateMachine->AddState<PlayerWaitTurnState>();
        _fsmStates.PlayerPlayTurnState = _finiteStateMachine->AddState<PlayerPlayTurnState>();
        _fsmStates.PlayerDeadState     = _finiteStateMachine->AddState<PlayerDeadState>();
        _fsmStates.PlayerWinState      = _finiteStateMachine->AddState<PlayerWinState>();

        //Transition
        _finiteStateMachine->AddTransition<PlayerWaitTurnState, PlayerStartCondition, PlayerPlayTurnState>();
        _finiteStateMachine->AddTransition<PlayerPlayTurnState, PlayerExitCondition, PlayerWaitTurnState>();

        _finiteStateMachine->AddTransition<PlayerDeadCondition, PlayerDeadState>();
        _finiteStateMachine->AddTransition<PlayerDeadState, PlayerExitCondition, PlayerWaitTurnState>();

        _finiteStateMachine->AddTransition<PlayerWinCondition, PlayerWinState>();

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

void Player::OnKill(CharacterBase* destination)
{
    Base::OnKill(destination);
}

void Player::OnTokenAdded(const int tokenID)
{
    Base::OnTokenAdded(tokenID);
}

void Player::OnTokenRemoved(const int tokenID)
{
    Base::OnTokenRemoved(tokenID);
}

void Player::OnTokenEnter(int tokenID)
{
    Base::OnTokenEnter(tokenID);
}

void Player::OnTokenExit(int tokenID)
{
    Base::OnTokenExit(tokenID);
}

void Player::OnQTEStart() 
{
    Base::OnQTEStart();
}

void Player::OnQTEEnd() 
{
    Base::OnQTEEnd();
}

void Player::OnNotifiedAnimationEvent(const Timeline::EventContext* context)
{
    auto* modelTransform = transform->Find(MODEL_NAME);
    if (nullptr == modelTransform)
        return;
    auto particlecomponent = modelTransform->gameObject->GetComponent<ParticleComponent>();
    if (nullptr == particlecomponent)
        return;
    if ("castingStart" == context->GetLabel())
    {
        particlecomponent->PlayEffect("handglow");
    }
    if ("attackEnd" == context->GetLabel())
    {
        particlecomponent->StopEffect("handglow");
    }
}
