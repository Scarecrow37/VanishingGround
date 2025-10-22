#include "pchScripts.h"
#include "Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Enemy/EnemyStatsComponent.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "Particle/ParticleComponent.h"

//Condition
#include "Condition/EnemyStartCondition.h"
#include "Condition/EnemyEndCondition.h"
#include "Condition/EnemyDeadCondition.h"

//State
#include "State/EnemyWaitTurnState.h"
#include "State/EnemyPlayTurnState.h"
#include "State/EnemyDeadState.h"

// Stats
#include "Stats/CharacterStats.h"

#include "Monster/System/MonsterSystem.h"
#include "Monster/Action/MonsterActionBase.h"

UMREAL_COMPONENT(Enemy)

Enemy::Enemy() = default;

Enemy::~Enemy() = default;

void Enemy::PlayTurn() 
{
    Base::PlayTurn();
}

void Enemy::ImGuiDrawPropertysEvent()
{
    Base::ImGuiDrawPropertysEvent();
   
    if (ImGui::TreeNodeEx("Monster Controller##enemy component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        Monster::Controller& controller = GetController();
        ImGui::PushID(&controller);

        const Monster::AIModel& aiModel = controller.GetAIModel();
        ImGui::BulletText("Current FSM:");
        ImGui::Text("       ID: %d", controller.GetFSMID());
        ImGui::Text("       Current Node: %s", aiModel.GetCurrentNodeLabel());

        if (auto* currAction = controller.GetCurrentAction())
        {
            ImGui::BulletText("Current Action:");
            ImGui::Text("       ID: %d", currAction->GetActionID());
            ImGui::Text("       Name: %s", currAction->GetActionContext().Name.c_str());
            ImGui::Text("       Type: %s", currAction->GetActionContext().Type.c_str());
            ImGui::Text("       Target: %s", currAction->GetActionContext().Target.c_str());
            ImGui::Text("       Attack Count: %d", currAction->GetActionContext().AttackCount);
            ImGui::Text("       Parameter: %s", currAction->GetActionContext().Parameter.c_str());
        }
        else
        {
            ImGuiHelper::StyleBuilder styleBuilder;
            styleBuilder.PushStyleColor(ImGuiCol_Text, IM_COL32(255, 100, 100, 255));
            ImGui::TextUnformatted("Null Current Action");
        }
        ImGui::PopID();
        ImGui::TreePop();
    }
}

void Enemy::EndTurn() 
{
    Base::EndTurn();
}

void Enemy::Revive() 
{
    Base::Revive();
}

void Enemy::Dead()
{
    Base::Dead();
    if (auto turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        turnMode->ApplyActions([this](TurnAction& action) { action.OnEnemyDead(*this); });
    }
}

void Enemy::TakeDamage(int damage, bool playAnim) 
{
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    if (turnMode)
    {
        turnMode->ApplyActions([&](TurnAction& action) { action.OnEnemyTakeDamageStart(*this, damage); });
    }
    Base::TakeDamage(damage, playAnim);
    if (turnMode)
    {
        turnMode->ApplyActions([&](TurnAction& action) { action.OnEnemyTakeDamageEnd(*this, damage); });
    }
}

void Enemy::TakeDamage(int damage, const QTE::NoteResult& result, bool playAnim)
{
    // 혹시나 그럴 일 없겠지만 중간에 계산할 연산이 또 있다면 재연산
    int takeDamage = damage;
    Base::TakeDamage(takeDamage, result, playAnim);
    ParticleComponent* particle = GetParticleComponent();
    if (particle && result.IsHit())
    {
        particle->PlayEffect("normalhit");
    }
}

void Enemy::Awake()
{
    Base::Awake();
    gameObject->AddTag(TAG);

    BuildEnemyFSM();
    if (nullptr == GetEnemyStats())
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, (const char*)u8"Enemy Stats를 추가해주세요");
    }
}

void Enemy::Update() 
{
}

CharacterStats* Enemy::GetCharacterStats()
{
    CharacterStats* stats = nullptr;
    EnemyStatsComponent* statsComponent = GetEnemyStats();
    if (nullptr != statsComponent)
    {
        stats = &statsComponent->GetStats();
    }
    return stats;
}

int Enemy::GetSpeed()
{
    int speed = 0;
    EnemyStatsComponent* stats = GetEnemyStats();
    if (nullptr != stats)
    {
        speed = stats->GetStats().Speed;
    }
    return speed;
}

int Enemy::GetRandomSpeed()
{
    return _randomSpeed;
}

void Enemy::SetPositionFromSpawnPoint(Monster::SpawnPoint spawnPointType) 
{
    if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
    {
        auto weakSpawnPoint = system->GetSpawnPointObject(spawnPointType);
        if (auto spawnPoint = weakSpawnPoint.lock())
        {
            Vector3 spawnLocal    = spawnPoint->transform->Position;
            Vector3 spawnEuler    = spawnPoint->transform->EulerAngle;
            transform->Position   = spawnLocal;
            transform->EulerAngle = spawnEuler;
            _spawnPoint           = spawnPointType;
        }
    }
}

EnemyStatsComponent* Enemy::GetEnemyStats()
{
    if (nullptr == _enemyStats)
    {
        _enemyStats = GetComponent<EnemyStatsComponent>();
        if (nullptr == _enemyStats)
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Enemy Stats 컴포넌트가 존재하지 않습니다.");
        }
    }
    return _enemyStats;
}

void Enemy::SetMonsterHUD(GameObject* HUD) 
{
    _monsterHUD = HUD;
}

void Enemy::BuildEnemyFSM()
{
    _finiteStateMachine = GetComponent<FiniteStateMachine>();
    if (nullptr == _finiteStateMachine)
    {
        _finiteStateMachine = &AddComponent<FiniteStateMachine>();

        //Condition
        _finiteStateMachine->AddCondition<EnemyStartCondition>();
        _finiteStateMachine->AddCondition<EnemyEndCondition>();
        _finiteStateMachine->AddCondition<EnemyDeadCondition>();

        //State
        _fsmStates.WaitTurn = _finiteStateMachine->AddState<EnemyWaitTurnState>();
        _fsmStates.PlayTurn = _finiteStateMachine->AddState<EnemyPlayTurnState>();
        _fsmStates.Dead     = _finiteStateMachine->AddState<EnemyDeadState>();

        //Transition
        _finiteStateMachine->AddTransition<EnemyWaitTurnState, EnemyStartCondition, EnemyPlayTurnState>();
        _finiteStateMachine->AddTransition<EnemyPlayTurnState, EnemyEndCondition, EnemyWaitTurnState>();
        _finiteStateMachine->AddTransition<EnemyDeadCondition, EnemyDeadState>();

        //Entry
        _finiteStateMachine->SetEntryState<EnemyWaitTurnState>();
    }
}

void Enemy::OnCombatStart()
{
    Base::OnCombatStart();
}

void Enemy::OnRoundStart()
{
    // 라운드 Start 이전에 Random Speed를 뽑아둠.
    _randomSpeed = Random::Range(DEFINE::RANDOMSPEED_MIN, DEFINE::RANDOMSPEED_MAX);
    TokenInventory& tokenInventory = GetTokenInventory();
    tokenInventory.NotifyRollRandomSpeed(_randomSpeed);
    Base::OnRoundStart();
}

void Enemy::OnRoundEnd()
{
    Base::OnRoundEnd();
}

void Enemy::OnEachTurnStart(CharacterBase* destination)
{
    Base::OnEachTurnStart(destination);
}

void Enemy::OnTurnStart()
{
    Base::OnTurnStart();
}

void Enemy::OnTurnEnd()
{
    Base::OnTurnEnd();
    // Enemy의 턴이 종료시 액션을 선언.
    _controller.Transition();
}

void Enemy::OnHit()
{
    Base::OnHit();
}

void Enemy::OnKill(CharacterBase* destination)
{
    Base::OnKill(destination);
}

void Enemy::OnTokenAdded(int tokenID)
{
    Base::OnTokenAdded(tokenID);
}

void Enemy::OnTokenRemoved(int tokenID)
{
    Base::OnTokenRemoved(tokenID);
}

void Enemy::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
{
    if (_fsmStates.WaitTurn && STATE::Wait == State)
    {
        _fsmStates.WaitTurn->OnNotifiedAnimationEvent(context);
    }
    if (_fsmStates.PlayTurn && STATE::Play == State)
    {
        _fsmStates.PlayTurn->OnNotifiedAnimationEvent(context);
    }
    if (_fsmStates.Dead && STATE::Dead == State)
    {
        _fsmStates.Dead->OnNotifiedAnimationEvent(context);
    }
}