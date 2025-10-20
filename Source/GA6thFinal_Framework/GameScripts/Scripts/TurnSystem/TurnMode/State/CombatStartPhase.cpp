#include "pchScripts.h"
#include "CombatStartPhase.h"

#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnAction/TurnAction.h"
#include "RevelationSystem/RevelationSystem.h"
#include "WeaponSystem/WeaponSystem.h"
#include "AccessorySystem/AccessorySystem.h"
#include "PlayerSystem/PlayerSystem.h"

#include "Scripts/Stats/Enemy/EnemyStatsComponent.h"
#include "UI/Views/MonsterHp/MonsterHpView.h"
#include "UI/Views/MonsterChain/MonsterChainView.h"

#include "CombatUIManager/CombatUIManager.h"
#include "QTE/UI/QTEUIManager.h"

REGISTER_CLASS(FSMStateFactory, CombatStartPhase)

static constexpr int EXPECTED_ENEMY_COUNT = 3;

CombatStartPhase::CombatStartPhase()
    : 
    _phaseEnd(false), 
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
                else if (typeid(Enemy) == type)
                {
                    _enemies.push_back(static_cast<Enemy*>(character));
                }
            }
        }
    }
    for (auto& character : _characters)
    {
        if (character)
        {
            character->ClearState();
        }
    }
}
#include "Monster/System/MonsterSystem.h"

void CombatStartPhase::OnAwake() 
{
    if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
    {
        // TODO: 나중에 전투에 맞는 스폰 ID로 변경
        Monster::SpawnID spawnID = 211111;
        system->SpawnMonsterFromSpawnID(spawnID);
    }
    ResetCharacterStats();
    SortEnemies();
    RegisterEnemiesHUD();
    RegisterEnemiesHP();
    RegisterEnemiesChain();
    ReviveEnemies();
    ResetPlayer();
    RefreshUI();
}

void CombatStartPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}
void CombatStartPhase::OnEnter() 
{
    /// 사운드
    UmAudio.Play("-20000");

    _turnMode->ResetRoundCount();
    AddValidActions();

    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...3");
    UmTime.Invoke(&GetFSM(), 1.f, [this]() { UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...2"); });
    UmTime.Invoke(&GetFSM(), 2.f, [this]() { UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작...1"); });
    UmTime.Invoke(&GetFSM(), 3.f, [this]() 
    { 
        this->_phaseEnd = true; 
        
        if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
        {
            combatUIManager->SetActiveUI(true);
        }
    });

    NotifyCombatStart();
    Battle::ResetLastCharacter();
}

void CombatStartPhase::OnExit() 
{

}

void CombatStartPhase::OnUpdate() 
{

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
            TurnAction* action = accessory.GetAction();
            if (action)
            {
                _turnMode->AddTurnAction(action);
            }
        }     
    }
}

void CombatStartPhase::SortEnemies() 
{
    if (_enemies.size() != EXPECTED_ENEMY_COUNT)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"적이 3명이 아닙니다.");
        return;
    }

    // 무게 중심 계산하기
    Vector3 centroid;
    for (auto& enemy : _enemies)
    {
        centroid += enemy->transform->Position;
    }
    centroid /= (float)_enemies.size();

    //플레이어 -> 무게중심 방향 백터
    Vector3 referDir = centroid - _player->transform->Position;
    referDir.Normalize();

    // 가장 작은 각도를 이루는 적 찾기 (가운데 적 찾기)
    Enemy* centerEnemy = nullptr;
    float  maxDot      = -2.f;
    for (auto& enemy : _enemies)
    {
        // 플레이어 -> 적 방향 백터
        Vector3 toEnemyDir = enemy->transform->Position - _player->transform->Position;
        toEnemyDir.Normalize();

        // 두 방향 백터 내적해서 시야각 계산
        float dot = referDir.Dot(toEnemyDir);
        if (dot > maxDot)
        {
            maxDot = dot;
            centerEnemy = enemy;
        }        
    }

    if (centerEnemy)
    {
        // 가운데 적 기준 왼쪽, 오른쪽 찾기
        std::vector<Enemy*> otherEnemies;
        otherEnemies.reserve(2);
        for (auto& enemy : _enemies)
        {
            if (centerEnemy != enemy)
            {
                otherEnemies.push_back(enemy);
            }
        }

        Enemy*  leftEnemy      = nullptr;
        Enemy*  rightEnemy     = nullptr;
        Vector3 playerToCenter = centerEnemy->transform->Position - _player->transform->Position;
        Vector3 playerToOther  = otherEnemies[0]->transform->Position - _player->transform->Position;

        // 외적 수행 판별
        float crossY = playerToCenter.Cross(playerToOther).y;
        if (crossY < 0)
        {
            leftEnemy  = otherEnemies[0];
            rightEnemy = otherEnemies[1];
        }
        else
        {
            leftEnemy  = otherEnemies[1];
            rightEnemy = otherEnemies[0];
        }
        _enemies[0] = leftEnemy;
        _enemies[1] = centerEnemy;
        _enemies[2] = rightEnemy;
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
    auto SetHUDObject = [&](size_t index, const std::string& tag) 
    {
        if (index < _enemies.size())
        {
            const std::weak_ptr<GameObject> weakGameObject = GameObject::FindWithTag(tag);
            if (auto object = weakGameObject.lock())
            {
                _enemies[index]->SetMonsterHUD(object.get());
            }
        }
    };

    for (size_t i = 0; i < _enemies.size(); ++i)
    {
        if (EnemyStatsComponent* statsComponent = _enemies[i]->GetComponent<EnemyStatsComponent>())
        {
            statsComponent->RegisterHUD(HUD_KEY_ARRAY[i].data());
        }
    }

    SetHUDObject(0, "Left Monster HUD");
    SetHUDObject(1, "Middle Monster HUD");
    SetHUDObject(2, "Right Monster HUD");
}

void CombatStartPhase::RegisterEnemiesHP() const
{
    RegisterEnemyHP(0, HUD_KEY_ARRAY[0].data(), "Left Monster HP UI");
    RegisterEnemyHP(1, HUD_KEY_ARRAY[1].data(), "Middle Monster HP UI");
    RegisterEnemyHP(2, HUD_KEY_ARRAY[2].data(), "Right Monster HP UI");
}

void CombatStartPhase::RegisterEnemyHP(const int index, const std::string& key, const std::string& tag) const
{
    if (index < _enemies.size())
    {
        if (const EnemyStatsComponent* leftEnemyStatsComponent = _enemies[index]->GetComponent<EnemyStatsComponent>();
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
            UmLogger.Log(LogLevel::LEVEL_ERROR,
                         "EnemyStatsComponent not found for enemy at index " + std::to_string(index));
        }
    }
}

void CombatStartPhase::RegisterEnemiesChain() 
{
    RegisterEnemyChain(0, HUD_KEY_ARRAY[0].data(), "Left Monster Chain UI");
    RegisterEnemyChain(1, HUD_KEY_ARRAY[1].data(), "Middle Monster Chain UI");
    RegisterEnemyChain(2, HUD_KEY_ARRAY[2].data(), "Right Monster Chain UI");
}

void CombatStartPhase::RegisterEnemyChain(int index, const std::string& key, const std::string& tag) 
{
    if (index < _enemies.size())
    {
        if (const EnemyStatsComponent* leftEnemyStatsComponent = _enemies[index]->GetComponent<EnemyStatsComponent>();
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
            UmLogger.Log(LogLevel::LEVEL_ERROR,
                         "EnemyStatsComponent not found for enemy at index " + std::to_string(index));
        }
    }
}

void CombatStartPhase::ReviveEnemies()
{
    for (auto& enemy : _enemies)
    {
        enemy->CharacterBase::Revive();
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

void CombatStartPhase::RefreshUI() 
{
    if (CombatUIManager* manager = SingletonComponent<CombatUIManager>::GetInstance())
    {
        manager->Refresh();
    }
    if (QTEUIManager* uiManager = QTEUIManager::GetInstance())
    {
        uiManager->Refresh();
        uiManager->SetUIAlpha(0.0f);
        uiManager->SetBackgroundUIAlpha(0.0f);
    }
}
