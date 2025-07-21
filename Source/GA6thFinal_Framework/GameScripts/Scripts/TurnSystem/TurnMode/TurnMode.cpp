#include "pchScripts.h"
#include "TurnMode.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include "TurnSystem/TurnActor/TurnActor.h"
#include <WeaponSystem/WeaponSystem.h>
#include <DamageSystem/DamageSystem.h>

//Condition
#include "GameCore/FSM/AlwaysTransitionCondition.h"
#include "Condition/RoundStartCondition.h"
#include "Condition/PlayerActionCondition.h"
#include "Condition/EnemyActionCondition.h"
#include "Condition/CheckRoundStartExit.h"
#include "Condition/CheckRoundEndExit.h"
#include "Condition/CheckTurnEndCondition.h"
#include "Condition/CheckTurnEmpty.h"
#include "Condition/CheckTurnNotEmpty.h"
#include "Condition/GameOverCondition.h"
#include "Condition/GameClearCondition.h"

//State
#include "State/CombatStartPhase.h"
#include "State/RoundStartPhase.h"   
#include "State/RoundEndPhase.h"
#include "State/PlayerActionPhase.h"
#include "State/EnemyActionPhase.h"
#include "State/CheckPlayerState.h"
#include "State/TurnListEmptyState.h"
#include "State/GameOverState.h"
#include "State/GameClearState.h"

//Character
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include <Stats/Player/PlayerStatsComponent.h>
#include <Stats/Player/PlayerStats.h>
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include <Stats/Enemy/EnemyStatsComponent.h>
#include <Stats/Enemy/EnemyStats.h>

TurnMode::TurnMode() 
    : 
    _roundCount(0), 
    _currTurnActor(nullptr)
{

}
TurnMode::~TurnMode()
{
    if (static_instance == this)
    {
        static_instance = nullptr;
    }
}

void TurnMode::MakeTurnList() 
{
    _turnList.clear();
    auto players = GameObject::FindGameObjectsWithTag(Player::TAG);
    for (auto& weak : players)
    {
        if (false == weak.expired())
        {
            auto object = weak.lock();
            Player* player = object->GetComponent<Player>();
            if (nullptr != player)
            {
                for (int i = 0; i < WeaponSystem::EQUIP_WEAPONS_SIZE; i++)
                {
                    _turnList.emplace_back(i, player);
                }
            }
        }
    }

    auto enemies = GameObject::FindGameObjectsWithTag(Enemy::TAG);
    for (auto& weak : enemies)
    {
        if (false == weak.expired())
        {
            auto object = weak.lock();
            Enemy* enemy = object->GetComponent<Enemy>();
            if (nullptr != enemy)
            {
                _turnList.emplace_back(-1, enemy);
            }
        }
    }
}

void TurnMode::SortTurnList()
{
    if (false == _turnList.empty())
    {
        std::shuffle(_turnList.begin(), _turnList.end(), Random::GetEngine());
        std::sort(_turnList.begin(), _turnList.end(),
        [this](std::pair<int, TurnActor*>& turnSlotA, std::pair<int, TurnActor*>& turnSlotB) 
        {
            int speedA = GetRealRoundSpeed(turnSlotA);
            int speedB = GetRealRoundSpeed(turnSlotB);
            return speedA > speedB;
        });
    }
}

TurnActor* TurnMode::PopTurnList()
{
    _currTurnActor = nullptr;
    while (false == _turnList.empty())
    {
        auto& actorSlot = _turnList.front();
        auto& [slot, actor] = actorSlot;
        _currTurnActor      = actor;
        _turnList.pop_front();
        if (_currTurnActor->State == TurnActor::STATE::Wait)
        {
            if (true == IsPlayerActorSlot(actorSlot))
            {
                WeaponSystem* weaponSystem = WeaponSystem::GetInstance();
                if (weaponSystem)
                {
                    weaponSystem->SetCurrentWeaponSlot(slot);
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Weapon System이 존재하지 않습니다.");
                }        
            }
            break;
        }
        _currTurnActor = nullptr;
    }
    return _currTurnActor;
}

int TurnMode::GetPendingActorCount()
{ 
    std::erase_if(_turnList, [](const std::pair<int, TurnActor*>& pair) 
    { 
        const auto& [order, actor] = pair;
        return TurnActor::STATE::Dead == actor->GetActorState();
    });
    return (int)_turnList.size();
}

void TurnMode::BuildTurnModeFSM() 
{
    _finiteStateMachine = GetComponent<FiniteStateMachine>();
    if (nullptr == _finiteStateMachine)
    {
        _finiteStateMachine = &AddComponent<FiniteStateMachine>();

        //State
        _systemStates.CombatStartPhase   = _finiteStateMachine->AddState<CombatStartPhase>();
        _systemStates.RoundStartPhase    = _finiteStateMachine->AddState<RoundStartPhase>();
        _systemStates.RoundEndPhase      = _finiteStateMachine->AddState<RoundEndPhase>();
        _systemStates.PlayerActionPhase  = _finiteStateMachine->AddState<PlayerActionPhase>();
        _systemStates.EnemyActionPhase   = _finiteStateMachine->AddState<EnemyActionPhase>();
        _systemStates.CheckPlayerState   = _finiteStateMachine->AddState<CheckPlayerState>();
        _systemStates.TurnListEmptyState = _finiteStateMachine->AddState<TurnListEmptyState>();
        _systemStates.GameOverState      = _finiteStateMachine->AddState<GameOverState>();
        _systemStates.GameClearState     = _finiteStateMachine->AddState<GameClearState>();

        //Condition
        _finiteStateMachine->AddCondition<AlwaysTransitionCondition>();
        _systemConditions.RoundStartCondition   = _finiteStateMachine->AddCondition<RoundStartCondition>();
        _systemConditions.PlayerActionCondition = _finiteStateMachine->AddCondition<PlayerActionCondition>();
        _systemConditions.EnemyActionCondition  = _finiteStateMachine->AddCondition<EnemyActionCondition>();
        _systemConditions.CheckRoundStartExit   = _finiteStateMachine->AddCondition<CheckRoundStartExit>();
        _systemConditions.CheckRoundEndExit     = _finiteStateMachine->AddCondition<CheckRoundEndExit>();
        _systemConditions.CheckTurnEndCondition = _finiteStateMachine->AddCondition<CheckTurnEndCondition>();
        _systemConditions.CheckTurnEmpty        = _finiteStateMachine->AddCondition<CheckTurnEmpty>();
        _systemConditions.CheckTurnNotEmpty     = _finiteStateMachine->AddCondition<CheckTurnNotEmpty>();
        _systemConditions.GameOverCondition     = _finiteStateMachine->AddCondition<GameOverCondition>();
        _systemConditions.GameClearCondition    = _finiteStateMachine->AddCondition<GameClearCondition>();

        //Entry
        _finiteStateMachine->SetEntryState<CombatStartPhase>();

        //Transition    
        _finiteStateMachine->AddTransition<CombatStartPhase, RoundStartCondition, RoundStartPhase>();
        _finiteStateMachine->AddTransition<RoundStartPhase, CheckRoundStartExit, CheckPlayerState>();

        _finiteStateMachine->AddTransition<CheckPlayerState, PlayerActionCondition, PlayerActionPhase>();
        _finiteStateMachine->AddTransition<PlayerActionPhase, CheckTurnEndCondition, TurnListEmptyState>();

        _finiteStateMachine->AddTransition<CheckPlayerState, EnemyActionCondition, EnemyActionPhase>();
        _finiteStateMachine->AddTransition<EnemyActionPhase, CheckTurnEndCondition, TurnListEmptyState>();

        _finiteStateMachine->AddTransition<TurnListEmptyState, CheckTurnNotEmpty, CheckPlayerState>();

        _finiteStateMachine->AddTransition<TurnListEmptyState, CheckTurnEmpty, RoundEndPhase>();
        _finiteStateMachine->AddTransition<RoundEndPhase, CheckRoundEndExit, RoundStartPhase>();

        _finiteStateMachine->AddTransition<GameOverCondition, GameOverState>();
        _finiteStateMachine->AddTransition<GameClearCondition, GameClearState>();
    }
}

int TurnMode::GetRealRoundSpeed(const std::pair<int, TurnActor*>& turnActor)
{
    bool isPlayer = IsPlayerActorSlot(turnActor);
    auto& [slot, actor] = turnActor;
    int roundSpeed      = 0;
    if (isPlayer)
    {
        WeaponSystem* weaponSystem = WeaponSystem::GetInstance();
        if (weaponSystem)
        {
            roundSpeed = weaponSystem->GetRoundSpeedToSlot(slot);
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Weapon System이 존재하지 않습니다.");
        }
    }
    else
    {
        roundSpeed = actor->RoundSpeed;
    }
    return roundSpeed;
}

void TurnMode::Reset() 
{
    static_instance = this;
}

void TurnMode::Awake()
{
    BuildTurnModeFSM();
}

void TurnMode::ImGuiDrawPropertysEvent() 
{
    if(ImGui::TreeNodeEx("Current", ImGuiTreeNodeFlags_DefaultOpen))
    {
        TurnActor* actor = GetCurrTurnActor(); 
        if (nullptr != actor)
        {
            if (ImGui::BeginTable("Transition", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::PushID(actor);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupColumn("State");
                    ImGui::TableSetupColumn("Round Speed");
                    ImGui::TableHeadersRow();

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Selectable(actor->gameObject->ToString().data()))
                    {
                    }
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Selectable(typeid(*actor).name() + 6);
                    ImGui::TableSetColumnIndex(2);
                    TurnActor::STATE currState = actor->State;
                    ImGui::Text(rfl::enum_to_string(currState).data());
                    ImGui::TableSetColumnIndex(3);
                    int roundSpeed = actor->RoundSpeed;
                    ImGui::Text("%d", roundSpeed);
                }
                ImGui::PopID();
                ImGui::EndTable();
            } 
        }
        else
        {
            ImGui::Text(STR_NULL);
        }  
        ImGui::TreePop();
    }


    if (ImGui::TreeNodeEx("TurnList", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginTable("Transition", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("State");
            ImGui::TableSetupColumn("Round Speed");
            ImGui::TableHeadersRow();
            for (auto& turnSlot : _turnList)
            {
                auto& [slot, actor] = turnSlot;
                ImGui::PushID(actor);
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Selectable(actor->gameObject->ToString().data()))
                    {
                    }
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Selectable(typeid(*actor).name() + 6);
                    ImGui::TableSetColumnIndex(2);
                    TurnActor::STATE currState = actor->State;
                    ImGui::Text(rfl::enum_to_string(currState).data());
                    ImGui::TableSetColumnIndex(3);
                    int roundSpeed = GetRealRoundSpeed(turnSlot);
                    ImGui::Text("%d", roundSpeed);
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
}

void TurnMode::Battle::operator()(Player& attacker, Enemy& target) 
{
    TurnMode*             turnMode             = TurnMode::GetInstance();
    WeaponSystem*         weaponSystem         = WeaponSystem::GetInstance();
    PlayerStatsComponent* playerStatsComponent = attacker.GetPlayerStats();
    EnemyStatsComponent*  enemyStatsComponent  = target.GetEnemyStats();
    if (turnMode && weaponSystem && playerStatsComponent && enemyStatsComponent)
    {
        PlayerStats playerStats(playerStatsComponent->GetStats());
        WeaponStats weaponStats(weaponSystem->GetCurrentWeaponStats());
        EnemyStats  enemyStats(enemyStatsComponent->GetStats());
        turnMode->ApplyActions([&](TurnAction& action) {
            action.OnPlayerBattleStart(attacker, playerStats, weaponStats, target, enemyStats);
        });

        int damage = DamageSystem::CalculateDamage(playerStats, weaponStats, enemyStats);
        target.TakeDamage(damage);
    }
}

void TurnMode::Battle::operator()(Enemy& attacker, Player& target) 
{
    TurnMode*             turnMode             = TurnMode::GetInstance();
    EnemyStatsComponent*  enemyStatsComponent  = attacker.GetEnemyStats();
    PlayerStatsComponent* playerStatsComponent = target.GetPlayerStats();
    if (turnMode && playerStatsComponent && enemyStatsComponent)
    {
        EnemyStats  enemyStats(enemyStatsComponent->GetStats());
        PlayerStats playerStats(playerStatsComponent->GetStats());
        turnMode->ApplyActions(
            [&](TurnAction& action) { action.OnEnemyBattleStart(attacker, enemyStats, target, playerStats); });

    }
}
