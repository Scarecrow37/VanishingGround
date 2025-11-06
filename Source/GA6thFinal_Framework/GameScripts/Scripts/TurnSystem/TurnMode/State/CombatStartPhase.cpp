#include "pchScripts.h"
#include "CombatStartPhase.h"

#include "Camera/UmCineMotion.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnAction/TurnAction.h"
#include "RevelationSystem/RevelationSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "AccessorySystem/AccessorySystem.h"
#include "PlayerSystem/PlayerSystem.h"
#include "ItemDropSystem/ItemDropSystem.h"

#include "Scripts/Stats/Enemy/EnemyStatsComponent.h"
#include "UI/Views/MonsterHp/MonsterHpView.h"
#include "UI/Views/MonsterChain/MonsterChainView.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "BattleIntroUIController/BattleIntroUIController.h"
#include "KeyCallbackUINavi/KeyCallbackUINavi.h"

#include "CombatUIManager/CombatUIManager.h"
#include "QTE/UI/QTEUIManager.h"

#include "Monster/System/MonsterSystem.h"

#include "DifficultyManager/DifficultyManager.h"
#include "Map/MapManager.h"
#include "Map/Stage.h"
#include "TutorialSystem/TutorialSystem.h"

REGISTER_CLASS(FSMStateFactory, CombatStartPhase)

static constexpr int EXPECTED_ENEMY_COUNT = 3;

CombatStartPhase::CombatStartPhase()
    : 
    _phaseEnd(false), 
    _waitPhaseEnd(false),
    _player(nullptr)
{

}

CombatStartPhase::~CombatStartPhase() 
{
   
}

void CombatStartPhase::ResetCharacterStats()
{
    _player = nullptr;
    _enemies.clear();
    _characters.clear();
    _spawnPointEnemyTable.clear();
    // 씬에 존재하는 모든 캐릭터 추가
    for (auto& weak : GameObject::FindGameObjectsWithTag(CharacterBase::TAG))
    {
        if (false == weak.expired())
        {
            auto           object    = weak.lock();
            CharacterBase* character = nullptr;
            for (int i = 0; i < object->GetComponentCount(); ++i)
            {
                character = object->GetComponentAtIndex<CharacterBase>(i);
                if (nullptr != character)
                {
                    break;
                }
            }
            if (nullptr != character)
            {
                const auto& type = typeid(*character);
                _characters.push_back(character);
                if (typeid(Player) == type)
                {
                    _player = static_cast<Player*>(character);
                }
            }
        }
    }

    if (MonsterSystem* monsterSystem = SingletonComponent<MonsterSystem>::GetInstance())
    {
        const auto& spawnedEnemiesTable = monsterSystem->GetSpawnedEnemiesTable();
        for (const auto& [spawnPoint, weakEnemy] : spawnedEnemiesTable)
        {
            if (Monster::SpawnPoint::Invalid == spawnPoint)
            {
                continue;
            }
            if (auto enemy = weakEnemy.lock())
            {
                _characters.push_back(enemy.get());
                _enemies.push_back(enemy.get());
                _spawnPointEnemyTable[spawnPoint] = enemy.get();
            }
        }
        // 스폰 포인트 오름차순으로 정렬
        std::sort(_enemies.begin(), _enemies.end(), [](const Enemy* a, const Enemy* b) {
            Monster::SpawnPoint aSpawnPoint = a->SpawnPoint;
            Monster::SpawnPoint bSpawnPoint = b->SpawnPoint;
            return static_cast<int>(aSpawnPoint) < static_cast<int>(bSpawnPoint);
        });
    }

    for (auto& character : _characters)
    {
        if (character)
        {
            character->ClearState();
        }
    }
}

Enemy* CombatStartPhase::GetEnemyFromSpawnPoint(Monster::SpawnPoint spawnPoint) const
{
    auto iter = _spawnPointEnemyTable.find(spawnPoint);
    if (iter != _spawnPointEnemyTable.end())
    {
        return iter->second;
    }
    return nullptr;
}

void CombatStartPhase::OnAwake() 
{

}

void CombatStartPhase::OnStart() 
{
    TurnModeStateBase::OnStart();

    if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
    {
        Difficulty       difficulty = Difficulty::NORMAL;
        Monster::SpawnID spawnID    = 0;
        if (DifficultyManager* manager = SingletonComponent<DifficultyManager>::GetInstance())
        {
            difficulty = manager->GetDifficulty();
        }
        if (MapManager* manager = SingletonComponent<MapManager>::GetInstance())
        {
            spawnID = manager->GetCurrentSpawnID();
        }

        if (0 != spawnID)
            system->SpawnMonsterFromSpawnID(spawnID, difficulty);
        else
            system->SpawnMonsterFromSpawnID(211112);
    }
    ResetCharacterStats();
    RegisterEnemiesHUD();
    RegisterEnemiesNavi();
    RegisterEnemiesHP();
    RegisterEnemiesChain();

    AddValidActions();
    ReviveEnemies();
    ResetPlayer();
}
void CombatStartPhase::OnEnter() 
{
    _phaseEnd = false;
    _waitPhaseEnd = true;
    float introDuration = 0.f;
    if (ItemDropSystem* dropSystem = SingletonComponent<ItemDropSystem>::GetInstance())
    {
        if (UmCineMotion* introCamera = _turnMode->GetIntroCamera())
        {
            if (dropSystem->StageClearCount == 0)
            {
                introCamera->SetMainCamera();
                introCamera->ResetRail(true);
                introCamera->StartRail(false);
                introDuration = introCamera->Duration;
            }
            else
            {
                introCamera->ResetRail(false);
            }
        }
    }
    UmTime.Invoke(GetFSM(), introDuration, [this]() 
    { 
        _waitPhaseEnd = false;
    });
   
    _turnMode->ResetRoundCount();
    AddExtinctionRevelation();

    Battle::ResetLastCharacter();
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        system->FindRevelationsView();
    }

    NotifyCombatStart();
}

void CombatStartPhase::OnExit() 
{
    if (UmCineMotion* battleCamera = _turnMode->GetBattleCamera())
    {
        battleCamera->SetMainCamera();
        battleCamera->ResetRail(true);
    }

    if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
    {
        combatUIManager->CharacterHUDGroup.ActiveUI(true);
        if (auto focusNavi = GameObject::FindComponentWithTag<KeyCallbackUINavi>("Weapon Panel UI Navi").lock())
        {
            focusNavi->Focus();
        }
    }

    if (ItemDropSystem* itemDropSystem = SingletonComponent<ItemDropSystem>::GetInstance())
    {
        // 소멸 계시 추가시 튜토리얼
        int stageClearCount = itemDropSystem->StageClearCount;
        if (TutorialSystem* tutorial = SingletonComponent<TutorialSystem>::GetInstance())
        {
            if (0 < stageClearCount)
            {
                tutorial->Show(805908);
            }
        }      
    }
}

void CombatStartPhase::OnUpdate() 
{
    if (false == _phaseEnd && false == _waitPhaseEnd)
    {
        if (SceneTransitionComponent* transition = SingletonComponent<SceneTransitionComponent>::GetInstance())
        {
            if (false == transition->IsTransitioning())
            {
                if (BattleIntroUIController* controller = SingletonComponent<BattleIntroUIController>::GetInstance())
                {
                    if (MapManager* manager = SingletonComponent<MapManager>::GetInstance())
                    {
                        if (Stage* stage = manager->GetCurrentSelectedStage())
                        {
                            float delay = controller->PlayIntro(stage->MainLevel, stage->BattleCount);
                            UmTime.Invoke(GetFSM(), delay, [this]()
                            { 
                                _phaseEnd = true; 
                            });
                            _waitPhaseEnd = true;
                            return;
                        }           
                    }
                }         
            }
            else
            {
                return;
            }          
        }
        _phaseEnd = true;
    } 
}

void CombatStartPhase::NotifyCombatStart() 
{
    for (auto& character : _characters)
    {
        if (character)
        {
            character->OnCombatStart();
        }
    }

    _turnMode->ApplyActions([](TurnAction& action) 
    { 
         action.OnCombatStart();
    });
}


void CombatStartPhase::AddValidActions()
{
    //장신구 액션들 추가
    if (_accessorySystem && _turnMode)
    {
        for (auto& accessory : _accessorySystem->GetPlayerAccessoryItems())
        {
            const auto& actions = accessory.GetActions();
            for (auto& action : actions)
            {
                if (action)
                {
                    _turnMode->AddTurnAction(action.get());
                }             
            }
        }     
    }
}

void CombatStartPhase::AddExtinctionRevelation() const
{
    if (ItemDropSystem* itemDropSystem = SingletonComponent<ItemDropSystem>::GetInstance())
    {
        if (RevelationSystem* revelationSystem = SingletonComponent<RevelationSystem>::GetInstance())
        {
            //스테이지 클리어 횟수만큼 랜덤한 소멸 계시 추가
            int stageClearCount = itemDropSystem->StageClearCount;
            if (0 < stageClearCount)
            {
                revelationSystem->EquipRandomExtinctionElement(static_cast<size_t>(stageClearCount));       
            }           
        }
    }
}

namespace
{
    constexpr std::array<std::string_view, 3> HUD_KEY_ARRAY
    {
        "Left Monster HUD", 
        "Middle Monster HUD",
        "Right Monster HUD"
    };
}

void CombatStartPhase::RegisterEnemiesHUD() 
{
    auto SetEnemyHUD = [&](Monster::SpawnPoint point, const std::string& tag) 
    {
        const std::weak_ptr<GameObject> weakGameObject = GameObject::FindWithTag(tag);
        if (auto object = weakGameObject.lock())
        {
            if (Enemy* enemy = GetEnemyFromSpawnPoint(point))
            {
                object->ActiveSelf = true;
                enemy->SetMonsterHUD(object.get());
                if (EnemyStatsComponent* statsComponent = enemy->GetComponent<EnemyStatsComponent>())
                {
                    statsComponent->RegisterHUD(HUD_KEY_ARRAY[static_cast<size_t>(point)].data());
                }
            }
            else
            {
                object->ActiveSelf = false;
            }
        }
    };

    SetEnemyHUD(Monster::SpawnPoint::Left, "Left Monster HUD");
    SetEnemyHUD(Monster::SpawnPoint::Middle, "Middle Monster HUD");
    SetEnemyHUD(Monster::SpawnPoint::Right, "Right Monster HUD");
}

void CombatStartPhase::RegisterEnemiesNavi() 
{
    auto DisableNavi = [](Monster::SpawnPoint point) 
    {
        std::string tag;
        switch (point)
        {
        case Monster::SpawnPoint::Left:
            tag = "Enemy Left Panel UI Navi";
            break;
        case Monster::SpawnPoint::Middle:
            tag = "Enemy Middle Panel UI Navi";
            break;
        case Monster::SpawnPoint::Right:
            tag = "Enemy Right Panel UI Navi";
            break;
        default:
            break;
        }

        if (auto navi = GameObject::FindComponentWithTag<KeyCallbackUINavi>(tag).lock())
        {
            navi->Enable = false;
        }
    };

    if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
    {
        if (auto object = system->GetSpawnedEnemyFromSpawnPoint(Monster::SpawnPoint::Left).lock(); nullptr == object)
        {
            DisableNavi(Monster::SpawnPoint::Left);
        }
        if (auto object = system->GetSpawnedEnemyFromSpawnPoint(Monster::SpawnPoint::Middle).lock(); nullptr == object)
        {
            DisableNavi(Monster::SpawnPoint::Middle);
        }
        if (auto object = system->GetSpawnedEnemyFromSpawnPoint(Monster::SpawnPoint::Right).lock(); nullptr == object)
        {
            DisableNavi(Monster::SpawnPoint::Right);
        }
    }
}

void CombatStartPhase::RegisterEnemiesHP() const
{
    RegisterEnemyHP(0, HUD_KEY_ARRAY[0].data(), "Left Monster HP UI");
    RegisterEnemyHP(1, HUD_KEY_ARRAY[1].data(), "Middle Monster HP UI");
    RegisterEnemyHP(2, HUD_KEY_ARRAY[2].data(), "Right Monster HP UI");
}

void CombatStartPhase::RegisterEnemyHP(const int point, const std::string& key, const std::string& tag) const
{
    Monster::SpawnPoint spawnPoint = static_cast<Monster::SpawnPoint>(point);
    if (Enemy* enemy = GetEnemyFromSpawnPoint(spawnPoint))
    {
        if (const EnemyStatsComponent* leftEnemyStatsComponent = enemy->GetComponent<EnemyStatsComponent>();
            nullptr != leftEnemyStatsComponent)
        {
            const std::weak_ptr<GameObject> weakGameObject = GameObject::FindWithTag(tag);
            if (const auto sharedGameObject = weakGameObject.lock())
            {
                if (MonsterHpTextView* monsterHpView = sharedGameObject->GetComponent<MonsterHpTextView>())
                {
                    monsterHpView->Watch(key);
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_ERROR, "MonsterHpTextView with tag '" + tag + "' not found.");
                }

                if (MonsterHpImageView* monsterHpView = sharedGameObject->GetComponent<MonsterHpImageView>())
                {
                    monsterHpView->Watch(key);
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_ERROR, "MonsterHpImageView with tag '" + tag + "' not found.");
                }
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_ERROR, "GameObject with tag '" + tag + "' not found.");
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "EnemyStatsComponent not found for enemy at index " + rfl::enum_to_string(spawnPoint));
        }
    }        
}

void CombatStartPhase::RegisterEnemiesChain() 
{
    RegisterEnemyChain(0, HUD_KEY_ARRAY[0].data(), "Left Monster Chain UI");
    RegisterEnemyChain(1, HUD_KEY_ARRAY[1].data(), "Middle Monster Chain UI");
    RegisterEnemyChain(2, HUD_KEY_ARRAY[2].data(), "Right Monster Chain UI");
}

void CombatStartPhase::RegisterEnemyChain(int point, const std::string& key, const std::string& tag)
{
    Monster::SpawnPoint spawnPoint = static_cast<Monster::SpawnPoint>(point);
    if (Enemy* enemy = GetEnemyFromSpawnPoint(spawnPoint))
    {
        if (const EnemyStatsComponent* leftEnemyStatsComponent = enemy->GetComponent<EnemyStatsComponent>();
            nullptr != leftEnemyStatsComponent)
        {
            const std::weak_ptr<GameObject> weakGameObject = GameObject::FindWithTag(tag);
            if (const auto sharedGameObject = weakGameObject.lock())
            {
                if (MonsterChainTextView* monsterChainView = sharedGameObject->GetComponent<MonsterChainTextView>())
                {
                    monsterChainView->Watch(key);
                }
                else
                {
                    UmLogger.Log(LogLevel::LEVEL_ERROR, "MonsterChainTextView with tag '" + tag + "' not found.");
                }
            }
            else
            {
                UmLogger.Log(LogLevel::LEVEL_ERROR, "GameObject with tag '" + tag + "' not found.");
            }
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_ERROR, "EnemyStatsComponent not found for enemy at index " + rfl::enum_to_string(spawnPoint));
        }
    }
}

void CombatStartPhase::ReviveEnemies()
{
    for (auto& enemy : _enemies)
    {
        if (enemy)
        {
            enemy->Revive();
        }
    }
}

void CombatStartPhase::ResetPlayer() 
{
    PlayerSystem* playerSystem = SingletonComponent<PlayerSystem>::GetInstance();
    if (playerSystem)
    {
        playerSystem->SetStatsCombatStart();
    }

    //플레이어는 체력 회복하면 안됨.
    if (_player)
    {
        _player->TurnActor::Revive();
    }
}
