#include "TurnMode.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include "TurnSystem/TurnActor/TurnActor.h"

//Condition
#include "TurnSystem/Condition/CombatStartCodition.h"
#include "TurnSystem/Condition/RoundStartCondition.h"

//State
#include "TurnSystem/State/CombatStartPhase.h"
#include "TurnSystem/State/RoundStartPhase.h"   

//Character
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"

TurnMode::TurnMode() 
    : 
    _roundCount(0) 
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
    for (auto& actor : _turnList)
    {
        actor->SetRandomSpeed(Random::Range(0, 6));
    }
    std::sort(_turnList.begin(), _turnList.end(), [](TurnActor* actorA, TurnActor* actorB) 
    {
        int speedA = actorA->RoundSpeed;
        int speedB = actorB->RoundSpeed;
        if (speedA != speedB)
        {
            return speedA > speedB;
        }
        else
        {
            // Coin toss: 50% 확률로 actorA가 우선
            return Random::Range(0, 1) == 1;
        }      
    });
}

TurnActor* TurnMode::PopTurnList()
{
    TurnActor* turnMode = nullptr;
    if (false == _turnList.empty())
    {
        turnMode = _turnList.front();
        _turnList.pop_front();
    }
    return turnMode;
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

        //Condition
        _systemConditions.CombatStartCodition = _finiteStateMachine->AddCondition<CombatStartCodition>();
        _systemConditions.RoundStartCondition = _finiteStateMachine->AddCondition<RoundStartCondition>();

        //Entry
        _finiteStateMachine->SetEntryState<CombatStartPhase>();

        //Transition    
        _finiteStateMachine->AddTransition<CombatStartPhase, RoundStartCondition, RoundStartPhase>();
        
    }
}

void TurnMode::Awake() 
{
    BuildTurnModeFSM();
}

void TurnMode::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNodeEx("TurnList", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginTable("Transition", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
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
