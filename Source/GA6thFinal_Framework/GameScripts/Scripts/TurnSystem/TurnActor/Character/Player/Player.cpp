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
#include "AccessorySystem/AccessorySystem.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "TokenHUD/TokenHUD.h"
#include "Camera/UmCineMotion.h"
#include "Vinette/VinetteManager.h"
#include "KeyCallbackUINavi/KeyCallbackUINavi.h"
#include "TooltipSystem/TooltipSystem.h"

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

void Player::Start() 
{
    AddCallback();
}

void Player::AddCallback() 
{
    _callbacks.push_back(KeyCallbackUINavi::AddCallbackFocusIn("Player Navi", [this]() { FocusIn(); }));
    _callbacks.push_back(KeyCallbackUINavi::AddCallbackFocusOut("Player Navi", [this]() { FocusOut(); }));
    _callbacks.push_back(KeyCallbackUINavi::AddCallbackShowTooltips("Player Navi", [this]() { ShowTooltip(); }));
    _callbacks.push_back(KeyCallbackUINavi::AddCallbackHideTooltips("Player Navi", [this]() { HideTooltip(); }));
}

void Player::OnDestroy() 
{
    ClearCallback();
}

void Player::FocusIn()
{
    if (CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        combatUI->CharacterHUDGroup.PlayerFocusIn(0.2f);
    }
}

void Player::FocusOut()
{
    if (TooltipSystem* system = SingletonComponent<TooltipSystem>::GetInstance())
    {
        system->Hide();
    }
    if (CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        combatUI->CharacterHUDGroup.PlayerFocusOut(0.2f);
    }
}

void Player::ShowTooltip()
{
    if (TooltipSystem* system = SingletonComponent<TooltipSystem>::GetInstance())
    {
        auto&  tokenInventory = GetTokenInventory();
        std::vector<int> ids = tokenInventory.GetTokensTooltips();
        system->Show(Tooltip::Group::PLAYER, ids);
    }
}

void Player::HideTooltip()
{
    if (TooltipSystem* system = SingletonComponent<TooltipSystem>::GetInstance())
    {
        system->Hide();
    }
}

void Player::ClearCallback() 
{
    for (auto& [delegate, handle] : _callbacks)
    {
        delegate->RemoveListener(handle);
    }
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

    //인내의 가루 효과 그냥 강제 적용
    if (AccessorySystem* system = SingletonComponent<AccessorySystem>::GetInstance())
    {
        if (system->HasPlayerAccessory(203011))
        {
            takeDamage = std::min(takeDamage, 20);
        }
    }

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

    if (VinetteManager* vinette = SingletonComponent<VinetteManager>::GetInstance())
    {
        vinette->ShowHitVinette();
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
    if (VinetteManager* vinette = SingletonComponent<VinetteManager>::GetInstance())
    {
        vinette->ShowHealVinette();
    }
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

    RegisterTokenHUD(tokenID);
}

void Player::OnTokenExit(int tokenID)
{
    Base::OnTokenExit(tokenID);

    UnregisterTokenHUD(tokenID);
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

void Player::RegisterTokenHUD(int tokenID)
{
    auto&       tokenInventory = GetTokenInventory();
    const auto& inventory      = tokenInventory.GetValidTokenList();

    if (!inventory.empty())
    {
        auto& model = tokenInventory.GetTokenModelFromID(tokenID);

        // Assets/Prefab/UI/Token Icon.prefab
        if (auto prefab = UmGameObjectFactory.DeserializeToGuid("0abe19e7-1dc1-48cd-bcc1-6dcd86748d93"))
        {
            if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
            {
                auto HUD = combatUIManager->CharacterHUDGroup.PlayerHUDPanel;
                if (HUD)
                {
                    Transform::ForeachBFS(HUD->transform, [&](Transform* tr) {
                        GameObject& object = tr->gameObject;
                        if (object.CompareTag("Token HUD"))
                        {
                            if (auto tokenHUD = prefab->GetComponent<TokenHUD>())
                            {
                                if (TokenSystem* tokenSystem = SingletonComponent<TokenSystem>::GetInstance())
                                {
                                    if (const TokenData* tokenData = tokenSystem->GetTokenDataFromID(tokenID))
                                    {
                                        std::string key = std::format("Player_Token_{}", tokenID);
                                        tokenHUD->SetupTokenHUD(UmFileSystem.GetGuidFromAssetID(tokenData->ImageID),
                                                                model, key);
                                        _tokenHUDTable.emplace(tokenID, prefab.get());
                                        model.Notify();
                                        prefab->transform->SetParent(object.transform);
                                    }
                                }
                            }
                        }
                    });
                }              
            }
        }
    }
}

void Player::UnregisterTokenHUD(int tokenID)
{
    auto it = _tokenHUDTable.find(tokenID);
    if (it != _tokenHUDTable.end())
    {
        GameObject::Destroy(it->second);
        _tokenHUDTable.erase(it);
    }
}