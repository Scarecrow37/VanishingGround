#include "TurnMode.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include "TurnSystem/TurnActor/TurnActor.h"

//Condition
#include "GameCore/FSM/AlwaysTransitionCondition.h"
#include "Condition/CombatStartCodition.h"
#include "Condition/RoundStartCondition.h"
#include "Condition/PlayerActionCondition.h"
#include "Condition/EnemyActionCondition.h"
#include "Condition/CheckPlayerCondition.h"
#include "Condition/CheckTurnEndCondition.h"
#include "Condition/CheckTurnEmpty.h"
#include "Condition/CheckTurnNotEmpty.h"

//State
#include "State/CombatStartPhase.h"
#include "State/RoundStartPhase.h"   
#include "State/PlayerActionPhase.h"
#include "State/EnemyActionPhase.h"
#include "State/CheckPlayerState.h"
#include "State/TurnListEmptyState.h"

//Character
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

TurnMode::TurnMode() 
    : 
    _roundCount(0), 
    _currTurnActor(nullptr)
{

}
TurnMode::~TurnMode() = default;

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
                _turnList.push_back(player);
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
                _turnList.push_back(enemy);
            }
        }
    }
}

void TurnMode::SortTurnList()
{
    if (false == _turnList.empty())
    {
        for (auto& actor : _turnList)
        {
            actor->SetRandomSpeed(Random::Range(0, 6));
        }
        std::shuffle(_turnList.begin(), _turnList.end(), Random::GetEngine());
        std::sort(_turnList.begin(), _turnList.end(), [](TurnActor* actorA, TurnActor* actorB) 
        {
            int speedA = actorA->RoundSpeed;
            int speedB = actorB->RoundSpeed;
            return speedA > speedB;
        });
    }
}

TurnActor* TurnMode::PopTurnList()
{
    _currTurnActor = nullptr;
    while (false == _turnList.empty())
    {
        _currTurnActor = _turnList.front();
        _turnList.pop_front();
        if (_currTurnActor->State == TurnActor::STATE::Wait)
        {
            break;
        }
        _currTurnActor = nullptr;
    }
    return _currTurnActor;
}

void TurnMode::BuildTurnModeFSM() 
{
    _finiteStateMachine = GetComponent<FiniteStateMachine>();
    if (nullptr == _finiteStateMachine)
    {
        _finiteStateMachine = &AddComponent<FiniteStateMachine>();

        //State
        _systemStates.CombatStartPhase = _finiteStateMachine->AddState<CombatStartPhase>();
        _systemStates.RoundStartPhase  = _finiteStateMachine->AddState<RoundStartPhase>();
        _systemStates.PlayerActionPhase = _finiteStateMachine->AddState<PlayerActionPhase>();
        _systemStates.EnemyActionPhase  = _finiteStateMachine->AddState<EnemyActionPhase>();
        _systemStates.CheckPlayerState  = _finiteStateMachine->AddState<CheckPlayerState>();
        _systemStates.TurnListEmptyState = _finiteStateMachine->AddState<TurnListEmptyState>();

        //Condition
        _finiteStateMachine->AddCondition<AlwaysTransitionCondition>();
        _systemConditions.CombatStartCodition = _finiteStateMachine->AddCondition<CombatStartCodition>();
        _systemConditions.RoundStartCondition = _finiteStateMachine->AddCondition<RoundStartCondition>();
        _systemConditions.PlayerActionCondition = _finiteStateMachine->AddCondition<PlayerActionCondition>();
        _systemConditions.EnemyActionCondition  = _finiteStateMachine->AddCondition<EnemyActionCondition>();
        _systemConditions.CheckPlayerCondition  = _finiteStateMachine->AddCondition<CheckPlayerCondition>();
        _systemConditions.CheckTurnEndCondition = _finiteStateMachine->AddCondition<CheckTurnEndCondition>();
        _systemConditions.CheckTurnEmpty        = _finiteStateMachine->AddCondition<CheckTurnEmpty>();
        _systemConditions.CheckTurnNotEmpty     = _finiteStateMachine->AddCondition<CheckTurnNotEmpty>();

        //Entry
        _finiteStateMachine->SetEntryState<CombatStartPhase>();

        //Transition    
        _finiteStateMachine->AddTransition<CombatStartPhase, RoundStartCondition, RoundStartPhase>();
        _finiteStateMachine->AddTransition<RoundStartPhase, CheckPlayerCondition, CheckPlayerState>();

        _finiteStateMachine->AddTransition<CheckPlayerState, PlayerActionCondition, PlayerActionPhase>();
        _finiteStateMachine->AddTransition<PlayerActionPhase, CheckTurnEndCondition, TurnListEmptyState>();

        _finiteStateMachine->AddTransition<CheckPlayerState, EnemyActionCondition, EnemyActionPhase>();
        _finiteStateMachine->AddTransition<EnemyActionPhase, CheckTurnEndCondition, TurnListEmptyState>();

        _finiteStateMachine->AddTransition<TurnListEmptyState, CheckTurnNotEmpty, CheckPlayerState>();

    }
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
            for (auto& actor : _turnList)
            {
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
                    int roundSpeed = actor->RoundSpeed;
                    ImGui::Text("%d", roundSpeed);
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
}
