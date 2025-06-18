#include "TurnMode.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include "TurnSystem/TurnActor/TurnActor.h"

//Condition
#include "TurnSystem/Condition/BattleStartCodition.h"
#include "TurnSystem/Condition/RoundStartCondition.h"

//State
#include "TurnSystem/State/BattleStartPhase.h"
#include "TurnSystem/State/RoundStartPhase.h"   

TurnMode::TurnMode() = default;
TurnMode::~TurnMode() = default;

void TurnMode::BuildTurnModeFSM() 
{
    _finiteStateMachine = GetComponent<FiniteStateMachine>();
    if (nullptr == _finiteStateMachine)
    {
        _finiteStateMachine = &AddComponent<FiniteStateMachine>();

        //State
        _finiteStateMachine->AddState<BattleStartPhase>();
        _finiteStateMachine->AddState<RoundStartPhase>();

        //Condition
        _finiteStateMachine->AddCondition<BattleStartCodition>();
        _finiteStateMachine->AddCondition<RoundStartCondition>();

        //Entry
        _finiteStateMachine->SetEntryState<BattleStartPhase>();

        //Transition    
        _finiteStateMachine->AddTransition<BattleStartPhase, BattleStartCodition, RoundStartPhase>();
        
    }
}

void TurnMode::Reset() 
{
   
}

void TurnMode::Awake() 
{
    BuildTurnModeFSM();
}

void TurnMode::Start() 
{
    auto actors = GameObject::FindGameObjectsWithTag(TurnActor::TAG);
    for (auto& weak : actors)
    {
        if (false == weak.expired())
        {
            auto actor = weak.lock();
            TurnActor* turnActor = actor->GetComponent<TurnActor>();
            if (nullptr != turnActor)
            {
                _trunList.push_back(turnActor);
            }
        }
    }
}

void TurnMode::Update() 
{

}

void TurnMode::ImGuiDrawPropertysEvent() 
{
    if (ImGui::TreeNodeEx("TurnList", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& actor : _trunList)
        {
            if (ImGui::Selectable(actor->gameObject->ToString().data()))
            {

            }
        }
        ImGui::TreePop();
    }
}
