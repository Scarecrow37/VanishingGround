#include "pchScripts.h"
#include "TurnMode.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include "TurnSystem/TurnActor/TurnActor.h"
#include <WeaponSystem/WeaponSystem.h>
#include <DamageSystem/DamageSystem.h>
#include "TurnSystem/TurnAction/Condition/RoundOnceCondition/RoundOnceCondition.h"
#include "TurnSystem/TurnAction/TurnAction.h"
#include "RoundInfoUI/RoundInfoUIManager.h"
#include "Camera/UmCineMotion.h"

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
#include "ViewModels/TurnQueue/TurnQueueViewModel.h"
#include "ViewModels/Weapon/WeaponViewModel.h"

#include <Stats/Enemy/EnemyStatsComponent.h>
#include <Stats/Enemy/EnemyStats.h>

UMREAL_COMPONENT(TurnMode)

TurnMode::TurnMode() 
    : 
    _roundCount(0), 
    _currTurnActor(nullptr)
{
   
}
TurnMode::~TurnMode()
{

}

Player* TurnMode::GetPlayer()
{
    if (_systemStates.CombatStartPhase)
    {
        return _systemStates.CombatStartPhase->GetPlayer();
    }
    return nullptr;
}

const std::vector<Enemy*>& TurnMode::GetEnemies()
{
    if (_systemStates.CombatStartPhase)
    {
        return _systemStates.CombatStartPhase->GetEnemies();
    }
    static std::vector<Enemy*> emptyEnemies;
    return emptyEnemies;
}

const std::vector<CharacterBase*>& TurnMode::GetCharacters()
{
    if (_systemStates.CombatStartPhase)
    {
        return _systemStates.CombatStartPhase->GetCharacters();
    }
    static std::vector<CharacterBase*> emptyCharacters;
    return emptyCharacters;
}

Enemy* TurnMode::GetEnemyFromSpawnPoint(Monster::SpawnPoint spawnPoint)
{
    if (_systemStates.CombatStartPhase)
    {
        return _systemStates.CombatStartPhase->GetEnemyFromSpawnPoint(spawnPoint);
    }
    return nullptr;
}

void TurnMode::MakeTurnList() 
{
    std::deque<std::pair<int, TurnActor*>> turnList;
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
                    turnList.emplace_back(i, player);
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
                turnList.emplace_back(-1, enemy);
            }
        }
    }
    
    std::erase_if(turnList, [](std::pair<int, TurnActor*>& pair) 
    {
        auto& [slot, actor] = pair;
        if (actor)
        {
            return actor->IsDead();
        }
        return true;
    });

    if (false == turnList.empty())
    {
        std::ranges::shuffle(turnList, Random::GetEngine());
        std::ranges::sort(turnList, 
            [this](const std::pair<int, TurnActor*>& turnSlotA, const std::pair<int, TurnActor*>& turnSlotB) {
            const int speedA = GetRealRoundSpeed(turnSlotA);
            const int speedB = GetRealRoundSpeed(turnSlotB);
            return speedA > speedB;
            });
    }

    _playerWeaponCounter = 0;
    _turnList = std::move(turnList);
}

void TurnMode::EraseTurnListToDeadCharacter() 
{
    _turnList.erase_if([](std::pair<int, TurnActor*>& pair) 
    {
        auto& [slot, actor] = pair;
        if (actor)
        {
            return actor->IsDead();
        }
        return true;
    });
}

void TurnMode::StartFrontTurnActor()
{
    if (_currTurnActor)
    {
        UmLogger.Log(
            LogLevel::LEVEL_ERROR,
            u8"현재 턴이 끝나지 않았습니다. TurnMode::StartFrontTurnActor()를 호출하기 전에 FinishCurrentTurn()을 호출하세요.");
        return;
    }

    auto firstWaitActorIterator = _turnList.cbegin();
    for (const auto endIterator = _turnList.cend(); firstWaitActorIterator != endIterator; ++firstWaitActorIterator)
    {
        if (auto& [slot, actor] = *firstWaitActorIterator; actor->State == TurnActor::STATE::Wait)
        {
            break;
        }
    }

    if (firstWaitActorIterator != _turnList.cbegin())
    {
        _turnList.erase(_turnList.cbegin(), firstWaitActorIterator);
    }

    if (false == _turnList.empty())
    {
        _turnList.ModifyFront([this](auto& actorSlot) 
        {
            if (true == IsPlayerActorSlot(actorSlot))
            {
                if (WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance())
                {
                    weaponSystem->SetCurrentWeaponSlot(actorSlot.first);
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Weapon System이 존재하지 않습니다.");
                }
                ++_playerWeaponCounter;
            }
            _currTurnActor = actorSlot.second;
        });

    }
}

void TurnMode::FinishCurrentTurn()
{
    if (false == _turnList.empty())
    {
        _turnList.pop_front();
    }
    _currTurnActor = nullptr;
    _currentTurnRevelationActiveFlag = false;
}

int TurnMode::GetPendingActorCount()
{
    _turnList.erase_if([](const std::pair<int, TurnActor*>& pair) 
    { 
        const auto& [order, actor] = pair;
        return TurnActor::STATE::Dead == actor->GetActorState();
    });
    return static_cast<int>(_turnList.size());
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

void TurnMode::AddRoundOnceActions() 
{
    for (auto& action : RoundOnceTrueCondition::RoundOnceAction::_roundOnceActions)
    {
        AddTurnAction(action);
    }
}

int TurnMode::GetRealRoundSpeed(const std::pair<int, TurnActor*>& turnActor)
{
    bool isPlayer = IsPlayerActorSlot(turnActor);
    auto& [slot, actor] = turnActor;
    int roundSpeed      = 0;
    if (isPlayer)
    {
        WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
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

void TurnMode::CallAddedAction(TurnAction* action) 
{
    action->OnAddedAction();
}

void TurnMode::Reset()
{
    _singletonComponent.SetSingleTon();
}

void TurnMode::Awake()
{
    if (_singletonComponent.TrySingleTon())
    {
        UmWatcher.Register<TurnQueueViewModel>("Turn Queue", _turnList);
        UmWatcher.Register<WeaponViewModel>("Weapon", _currTurnActor);
    }
    BuildTurnModeFSM();
    AddRoundOnceActions();
    FindCameras();
    
}

void TurnMode::OnDestroy() 
{
    if (_singletonComponent.IsSingleTon())
    {
        _turnList.Reset();
        UmWatcher.Unregister<TurnQueueViewModel>("Turn Queue");
        UmWatcher.Unregister<WeaponViewModel>("Weapon");

        ApplyActions([this](TurnAction& action)
        {
            action.SetDestroy();
        });
    }
}

void TurnMode::ImGuiDrawPropertysEvent() 
{
    if(ImGui::TreeNodeEx("Current", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (auto& actorModel = GetCurrTurnActor(); nullptr != actorModel)
        {
            if (ImGui::BeginTable("Transition", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                ImGui::PushID(&actorModel);
                {
                    ImGui::TableSetupColumn("Name");
                    ImGui::TableSetupColumn("Type");
                    ImGui::TableSetupColumn("State");
                    ImGui::TableSetupColumn("Round Speed");
                    ImGui::TableHeadersRow();

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Selectable(actorModel->gameObject->ToString().data()))
                    {
                    }
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Selectable(typeid(*actorModel).name() + 6);
                    ImGui::TableSetColumnIndex(2);
                    TurnActor::STATE currState = actorModel->State;
                    ImGui::Text(rfl::enum_to_string(currState).data());
                    ImGui::TableSetColumnIndex(3);
                    int roundSpeed = actorModel->RoundSpeed;
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
            const auto& turnList = _turnList;
            for (auto& turnSlot : turnList)
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

    if(UmCore->IsPlay() && ImGui::TreeNode("UI Test"))
    {
        if (ImGui::Button("Play Round Fade UI"))
        {
            if (auto roundInfoUI = GameObject::FindComponentWithTag<RoundInfoUIManager>("Round Info Panel").lock())
            {
                std::string info = (const char*)u8"라운드  ";
                info += std::to_string(_roundCount);
                roundInfoUI->FadeInfoUI(info);
            }                            
        }
        ImGui::TreePop();
    }
}

void TurnMode::FindCameras() 
{
    if (CameraComponent* main = CameraComponent::MainCamera())
    {
        GameObject* group = (nullptr != main->transform->Root) ? &main->transform->Root->gameObject : &main->gameObject;
        std::vector<GameObject*> cameras = group->transform->FindBFSwithTag("Camera");    
        for (size_t i = 0; i < cameras.size(); ++i)
        {
            GameObject* object = cameras[i];
            if (object)
            {
                std::string_view objName = object->ToString();
                auto tolower = [](unsigned char c){ return std::tolower(c); };
                if (auto findIntro = std::ranges::search(objName, "intro", {}, tolower, tolower); findIntro.begin() != objName.end())
                {
                    if (UmCineMotion* motion = object->GetComponent<UmCineMotion>())
                    {
                        _introCamera = motion->GetWeakPtrAs<UmCineMotion>();
                    }                 
                }
                else if (auto findMain = std::ranges::search(objName, "main", {}, tolower, tolower); findIntro.begin() != objName.end())
                {
                    if (UmCineMotion* motion = object->GetComponent<UmCineMotion>())
                    {
                        _battleCamera = motion->GetWeakPtrAs<UmCineMotion>();
                    }  
                }
            }
        }
    }
}

