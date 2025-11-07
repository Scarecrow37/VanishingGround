#include "pchScripts.h"
#include "Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Enemy/EnemyStatsComponent.h"
#include "GameCore/FSM/FiniteStateMachine.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "Particle/ParticleComponent.h"
#include "Camera/UmCineMotion.h"
#include "RevelationSystem/RevelationSystem.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "TokenHUD/TokenHUD.h"
#include "TooltipSystem/TooltipSystem.h"
#include "KeyCallbackUINavi/KeyCallbackUINavi.h"
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
#include "CombatUIManager/CombatUIManager.h"
#include "Stats/CharacterStats.h"

#include "Monster/System/MonsterSystem.h"
#include "Monster/Action/MonsterActionBase.h"
#include "UI/Contents/SpawnDamagePanel.h"

// TurnAction
#include "TurnSystem/TurnAction/TurnActionFactory.h"
#include "ProclamationHUD/ProclamationHUD.h"

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

    ImGui::Separator();
    ImGuiHelper::AlignedText("Custom Action", ImGuiHelper::LEFT, 0.8f);
    ShowActionEditor();

    ImGui::Separator();
    ImGuiHelper::AlignedText("FSM", ImGuiHelper::LEFT, 0.8f);
    Monster::Controller& controller = GetController();
    ImGui::PushID(&controller);
    const Monster::AIModel& aiModel = controller.GetAIModel();
    ImGui::BulletText("Current FSM:");
    ImGui::Text("       ID: %d", controller.GetFSMID());
    ImGui::Text("       Current Node: %s", aiModel.GetCurrentNodeLabel());

    ImGui::Separator();
    ImGuiHelper::AlignedText("Action", ImGuiHelper::LEFT, 0.8f);
    if (auto* currAction = controller.GetCurrentAction())
    {
        ImGui::BulletText("Current Action:");
        ImGui::Text("       ID: %d", currAction->GetActionID());
        ImGui::Text("       Name: %s", currAction->GetActionContext().Name.c_str());
        ImGui::Text("       IconID: %d", currAction->GetActionContext().IconID);
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
}

void Enemy::SerializedReflectEvent()
{
    ReflectFields->Actions.clear();
    if (false == _actions.empty())
    {
        for (auto& action : _actions)
        {
            if (action)
            {
                const std::string& name = action->ActionName;
                std::string        data = action->SerializedReflectFields();
                ReflectFields->Actions.emplace_back(name, data);
            }
        }
    }
}

void Enemy::DeserializedReflectEvent()
{
    _actions.clear();
    if (false == ReflectFields->Actions.empty())
    {
        const auto& actionFactory = TurnActionFactory::GetActionFactory();
        for (auto& [name, data] : ReflectFields->Actions)
        {
            if (auto iter = actionFactory.find(name); iter != actionFactory.end())
            {
                auto& myAction = _actions.emplace_back();
                myAction.reset(iter->second());
                bool result = myAction->DeserializedReflectFields(data);
                if (false == result)
                {
                    _actions.pop_back();
                }
            }
        }
    }
}

void Enemy::OnDestroy() 
{
    ClearCallback();
}

void Enemy::ClearState() 
{
    Base::ClearState();
    // 초기 토큰 설정
    _controller.SetInitialToken();
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
    if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
    {
        if (auto HUD = combatUIManager->CharacterHUDGroup.EnemyActionPanel[static_cast<int>(_spawnPoint)])
        {
            HUD->gameObject->ActiveSelf = false;
        }
    }
}

void Enemy::TakeDamage(int damage, const bool playAnim) 
{
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    if (turnMode)
    {
        turnMode->ApplyActions([&](TurnAction& action) { action.OnEnemyTakeDamageStart(*this, damage); });
    }
    Base::TakeDamage(damage, playAnim);
    
    if (RevelationSystem* system = SingletonComponent<RevelationSystem>::GetInstance())
    {
        if (_isCriticalDamage)
        {
            ShowCriticalDamage(damage, system->GetBattleActiveRevelations());
        }
        else
        {
            ShowDamage(damage, system->GetBattleActiveRevelations());
        }
    }
    else
    {
        _isCriticalDamage ? ShowCriticalDamage(damage, {}) : ShowDamage(damage, {});
    }

    _isCriticalDamage = false;
    if (turnMode)
    {
        turnMode->ApplyActions([&](TurnAction& action) { action.OnEnemyTakeDamageEnd(*this, damage); });
    }
}

void Enemy::TakeDamage(const int damage, const QTE::NoteResult& result, const bool playAnim)
{
    GameObject& owner = gameObject;
    std::string spawnPoint = Monster::SpawnPointToString(SpawnPoint);
    ParticleComponent* particle = GetParticleComponent();

    if (true == IsDead() || false == result.IsHit())
    {
        std::string msg = std::format("{} {}{}", spawnPoint, owner.ToString(), (const char*)u8" 대한 공격 빗나감.");
        UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
        return;
    }
    switch (result.Result)
    {
    case QTE::QTE_RESULT_PERFECT: {
       
        std::string msg = std::format("{} {}{}", spawnPoint, owner.ToString(), (const char*)u8" 대한 공격 치명타!!");
        UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
        particle->StopEffect("criticalhit");
        particle->PlayEffect("criticalhit");
        if (TurnMode* mode = SingletonComponent<TurnMode>::GetInstance())
        {
            if (UmCineMotion* motion = mode->GetBattleCamera())
            {
                motion->BeginFeedBackShake(damage);
            }
        }
        _isCriticalDamage = true;
        break;
    }
    case QTE::QTE_RESULT_NORMAL: {
        std::string msg = std::format("{} {}{}", spawnPoint, owner.ToString(), (const char*)u8" 대한 공격 일격!!");
        UmLogger.Message(LogLevel::LEVEL_TRACE, msg);
        particle->StopEffect("normalhit");
        particle->PlayEffect("normalhit");
        _isCriticalDamage = false;
        break;
    }
    default:
        break;
    }
    TakeDamage(damage, playAnim);
}

void Enemy::ShowDamage(const int damage, const std::span<const std::string> sources)
{
    if (const CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        Monster::SpawnPoint   spawnPoint = SpawnPoint;
        const size_t          index      = static_cast<size_t>(spawnPoint);
        [[maybe_unused]] auto _ = combatUI->CharacterHUDGroup.EnemySpawnDamagePanel[index]->MakeDamage(damage, sources);
    }
}

void Enemy::ShowCriticalDamage(const int damage, const std::span<const std::string> sources)
{
    if (const CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        Monster::SpawnPoint   spawnPoint = SpawnPoint;
        const size_t          index      = static_cast<size_t>(spawnPoint);
        [[maybe_unused]] auto _ =
            combatUI->CharacterHUDGroup.EnemySpawnCriticalDamage[index]->MakeDamage(damage, sources);
    }
}

void Enemy::Heal(const int amount)
{
    Base::Heal(amount);
    ShowHeal(amount, {});
}

void Enemy::ShowHeal(const int healAmount, const std::span<const std::string> sources)
{
    if (const CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        Monster::SpawnPoint   spawnPoint = SpawnPoint;
        const size_t          index      = static_cast<size_t>(spawnPoint);
        [[maybe_unused]] auto _ = combatUI->CharacterHUDGroup.EnemySpawnHealPanel[index]->MakeDamage(healAmount, sources);
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

void Enemy::Start()
{
    Base::Start();
    switch (_spawnPoint)
    {
    case Monster::SpawnPoint::Left:
        AddCallback("Enemy Left Navi");
        break;
    case Monster::SpawnPoint::Middle:
        AddCallback("Enemy Middle Navi");
        break;
    case Monster::SpawnPoint::Right:
        AddCallback("Enemy Right Navi");
        break;
    default:
        break;
    }
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

void Enemy::SetPositionFromSpawnPoint(const Monster::SpawnPoint spawnPointType) 
{
    if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
    {
        auto weakSpawnPoint = system->GetSpawnPointObject(spawnPointType);
        if (auto spawnPoint = weakSpawnPoint.lock())
        {
            Vector3    spawnLocal    = spawnPoint->transform->Position;
            Quaternion spawnRotation = spawnPoint->transform->Rotation;
            Vector3    spawnScale    = spawnPoint->transform->Scale;
            transform->Position      = spawnLocal;
            transform->Rotation      = spawnRotation;
            transform->Scale         = spawnScale;
            _spawnPoint              = spawnPointType;
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
    for (auto& action : _actions)
    {
        if (action)
        {
            action->OnEnemyCombatStartPhase(*this);
        }
    }

    if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
    {
        if (auto HUD = combatUIManager->CharacterHUDGroup.EnemyActionPanel[static_cast<int>(_spawnPoint)])
        {
            HUD->gameObject->ActiveSelf = true;
            if (auto proclamationHUD = HUD->GetComponent<ProclamationHUD>())
            {
                _proclamationHUD = proclamationHUD;
                _proclamationHUD->FindUI();
            }
        }
    }    
}

void Enemy::OnRoundStart()
{
    // 라운드 Start 이전에 Random Speed를 뽑아둠.
    _randomSpeed = Random::Range(DEFINE::RANDOMSPEED_MIN, DEFINE::RANDOMSPEED_MAX);
    TokenInventory& tokenInventory = GetTokenInventory();
    tokenInventory.NotifyRollRandomSpeed(_randomSpeed);
    Base::OnRoundStart();

    SetupProclamationHUD();
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
    for (auto& action : _actions)
    {
        if (action)
        {
            action->OnEnemyTurnEnd(*this);            
        }
    }

    SetupProclamationHUD();
}

void Enemy::OnHit()
{
    Base::OnHit();
}

void Enemy::OnKill(CharacterBase* destination)
{
    Base::OnKill(destination);
}

void Enemy::OnTokenAdded(const int tokenID)
{
    Base::OnTokenAdded(tokenID);
}

void Enemy::OnTokenRemoved(const int tokenID)
{
    Base::OnTokenRemoved(tokenID);
}

void Enemy::OnTokenEnter(int tokenID)
{
    Base::OnTokenEnter(tokenID);

    RegisterTokenHUD(tokenID);
}

void Enemy::OnTokenExit(int tokenID)
{
    Base::OnTokenExit(tokenID);

    UnregisterTokenHUD(tokenID);
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

void Enemy::RegisterTokenHUD(int tokenID)
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
                int  monsterIndex = static_cast<int>(_spawnPoint);
                auto HUD          = combatUIManager->CharacterHUDGroup.EnemyHUDPanel[monsterIndex];
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
                                        std::string key = std::format("Enemy_{}_Token_{}", monsterIndex, tokenID);
                                        tokenHUD->SetupTokenHUD(UmFileSystem.GetGuidFromAssetID(tokenData->ImageID), model, key);
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

void Enemy::UnregisterTokenHUD(int tokenID)
{
    auto it = _tokenHUDTable.find(tokenID);
    if (it != _tokenHUDTable.end())
    {        
        GameObject::Destroy(it->second);
        _tokenHUDTable.erase(it);
    }
}

void Enemy::SetupProclamationHUD()
{
    if (auto action = _controller.GetCurrentAction())
    {
        if (_proclamationHUD)
        {
            const auto& context = action->GetActionContext();
            int         damage  = action->GetActionParam(1).Param;
            _proclamationHUD->SetDescriptionText(context.IconID, damage, (int)context.AttackCount);
        }
    }
}

void Enemy::ShowActionEditor() 
{
#ifdef _UMEDITOR
    static std::unordered_map<void*, bool> showEditorFlags;
    if (false == _actions.empty())
    {
        for (size_t i = 0; i < _actions.size(); ++i)
        {
            auto& action = _actions[i];
            bool& showEditor = showEditorFlags[&action];
            ImGui::PushID(&action);
            TurnAction::ImGuiDrawActionMaker("EnemyAction", action, showEditor);
            ImGui::PopID();
        }
    }
    if (ImGui::Button("Push Action"))
    {
        _actions.emplace_back();
    }
    ImGui::SameLine();
    if (ImGui::Button("Pop Action"))
    {
        if (false == _actions.empty())
        {
            showEditorFlags.erase(&_actions.back());
            _actions.pop_back();
        }
    }
#endif
}

void Enemy::AddCallback(const std::string& key)
{
    _callbacks.push_back(KeyCallbackUINavi::AddCallbackFocusIn(key, [this]() { FocusIn(); }));
    _callbacks.push_back(KeyCallbackUINavi::AddCallbackFocusOut(key, [this]() { FocusOut(); }));
    _callbacks.push_back(KeyCallbackUINavi::AddCallbackShowTooltips(key, [this]() { ShowTooltip(); }));
    _callbacks.push_back(KeyCallbackUINavi::AddCallbackHideTooltips(key, [this]() { HideTooltip(); }));
}

void Enemy::ClearCallback()
{
    for (auto& [delegate, handel] : _callbacks)
    {
        delegate->RemoveListener(handel);
    }
}

void Enemy::FocusIn()
{
    if (CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        combatUI->CharacterHUDGroup.MonsterFocusIn(_spawnPoint, 0.2f);
    }
}

void Enemy::FocusOut()
{
    if (TooltipSystem* system = SingletonComponent<TooltipSystem>::GetInstance())
    {
        system->Hide();
    }
    if (CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
    {
        combatUI->CharacterHUDGroup.MonsterFocusOut(_spawnPoint, 0.2f);
    }
}

void Enemy::ShowTooltip()
{
    if (TooltipSystem* system = SingletonComponent<TooltipSystem>::GetInstance())
    {
        auto&            tokenInventory = GetTokenInventory();
        std::vector<int> ids            = tokenInventory.GetTokensTooltips();
        system->Show(Tooltip::Group::ENEMY, ids);
    }
}

void Enemy::HideTooltip()
{
    if (TooltipSystem* system = SingletonComponent<TooltipSystem>::GetInstance())
    {
        system->Hide();
    }
}