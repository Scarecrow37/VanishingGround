#include "pchScripts.h"
#include "PlayerPlayTurnState.h"
#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>
#include <GameCore/FSM/FiniteStateMachine.h>
#include <Camera/UmCineMotion.h>

#include <QTE/System/QTESystem.h>
#include <QTE/UI/QTEUIManager.h>
#include <QTE/Track/QTETrack.h>
#include <WeaponSystem/WeaponSystem.h>
#include <WeaponModelManager/WeaponModelManager.h>

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

#include <CombatUIManager/CombatUIManager.h>


using namespace u8_literals;

REGISTER_CLASS(FSMStateFactory, PlayerPlayTurnState)

PlayerPlayTurnState::PlayerPlayTurnState() 
{
    _inputState               = InputState::NONE;
    _attackButtonHeldTime     = 0.f;
    _attackButtonHeldWaitTime = 1.0f;
    _attackRemaining          = 0;
    _isDownAButton            = false;
    _isDownAKey               = false;
}

PlayerPlayTurnState::~PlayerPlayTurnState() 
{

}

void PlayerPlayTurnState::OnAwake() 
{
    BindInputAction(ControllerButton::A, Action::PRESSED, &GetFSM(), this, &PlayerPlayTurnState::PressedButtonA);
    BindInputAction(ControllerButton::A, Action::RELEASED, &GetFSM(), this, &PlayerPlayTurnState::ReleasedButtonA);
}

void PlayerPlayTurnState::OnStart() 
{
}

void PlayerPlayTurnState::OnEnter() 
{
    _inputState           = InputState::ACTION_SELECTION;
    _attackButtonHeldTime = 0;
    _attackRemaining      = 0;

    if (QTEUIManager* qteUIManager = QTEUIManager::GetInstance())
    {
        qteUIManager->Refresh();
        qteUIManager->SetGuideNoteActive(true);
    }
}

void PlayerPlayTurnState::OnExit() 
{
    _inputState             = InputState::NONE;
    _attackRemaining        = 0;
    _attackButtonHeldTime   = 0.0f;
    _isDownAButton          = false;
}


void PlayerPlayTurnState::OnUpdate() 
{
    float dt = UmTime.DeltaTime();
    switch (_inputState)
    {
    case InputState::NONE:
        break;
    case InputState::ACTION_SELECTION:
        UpdateActionSelectionUI(dt);
        UpdateAttackButtonHeld(dt);
        break;
    case InputState::QUICK_TIME_EVENT:
        //UpdateQuickTimeEventUI(dt);
        break;
    case InputState::ATTACK_EVENT:
        UpdateAttackEventUI(dt);
        break;
    default:
        break;
    }

    if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
    {
        combatUIManager->Refresh();
    }
}

void PlayerPlayTurnState::PressedButtonA(const Input::Controller& controller)
{
    _isDownAButton = true;
    if (QTEUIManager* qteUIManager = QTEUIManager::GetInstance())
    {
        qteUIManager->StartShowQTEGuideNote();
    }
}

void PlayerPlayTurnState::ReleasedButtonA(const Input::Controller& controller)
{
    _isDownAButton = false;
    if (QTEUIManager* qteUIManager = QTEUIManager::GetInstance())
    {
        qteUIManager->StartHideQTEGuideNote();
    }
}

void PlayerPlayTurnState::UpdateAttackButtonHeld(float dt)
{
    if (_attackButtonHeldTime >= _attackButtonHeldWaitTime)
    {
        if (QTESystem* qteSystem = SingletonComponent<QTESystem>::GetInstance())
        {
            _inputState = InputState::QUICK_TIME_EVENT;
            qteSystem->StartQTE([this](const QTE::OverallResult& results) { SetAttack(); });
            SetAttackReady();
        }
        else
        {
            // 예외 처리
            Player& player = GetPlayer();
            player.EndTurn();
        }
    }
    _attackButtonHeldTime += _isDownAButton || _isDownAKey ? UmTime.DeltaTime() : -UmTime.DeltaTime();
    _attackButtonHeldTime = std::clamp(_attackButtonHeldTime, 0.f, _attackButtonHeldWaitTime);
}

void PlayerPlayTurnState::UpdateActionSelectionUI(float dt)
{
#ifdef _UMEDITOR
    _isDownAKey = ImGui::IsKeyDown(ImGuiKey_A); // 에디터에서는 키보드 인풋도 받음
#endif                                          // ISEDITOR

     Debugger()([this] {
        // 아래는 디버그용 코드입니다.
        ImGuiHelper::AlignedText("Combat", ImGuiHelper::LEFT, 0.8f);
        auto enemies = Battle::GetTargetsFromFlags(Battle::ENEMY_TARGET_FLAG_ALL);
        if (ImGui::Button((const char*)u8"[적] 전멸"))
        {
            for (auto& enemy : enemies)
            {
                if (enemy)
                    enemy->Dead();
            }
        }
        if (ImGui::Button((const char*)u8"[적 LEFT] 자살"))
        {
            if (enemies.size() >= 1 && enemies[0])
                enemies[0]->Dead();
        }
        ImGui::SameLine();
        if (ImGui::Button((const char*)u8"[적 MIDDLE] 자살"))
        {
            if (enemies.size() >= 2 && enemies[1])
                enemies[1]->Dead();
        }
        ImGui::SameLine();
        if (ImGui::Button((const char*)u8"[적 RIGHT] 자살"))
        {
            if (enemies.size() >= 3 && enemies[2])
                enemies[2]->Dead();
        }
        ImGui::SameLine();

        ImGui::Separator();
        Player& player = GetPlayer();
        if (ImGui::Button((const char*)u8"[플레이어] 자해"))
        {
            player.TakeDamage(10);
        }
        ImGui::SameLine();
        if (ImGui::Button((const char*)u8"[플레이어] 자살"))
        {
            player.Dead();
        }
        ImGui::SameLine();
        if (ImGui::Button((const char*)u8"[플레이어] 턴 종료"))
        {
            player.EndTurn();
        }
    });

    QTESystem*    qteSystem    = SingletonComponent<QTESystem>::GetInstance();
    QTEUIManager* qteUIManager = QTEUIManager::GetInstance();
    if (qteSystem && qteUIManager)
    {
        float t     = _attackButtonHeldTime / _attackButtonHeldWaitTime;
        bool input  = _isDownAKey || _isDownAButton;
        qteUIManager->SetBackgroundUIAlpha(t);
        qteUIManager->SetGuideNoteUIAlpha(1.0f - t);
        qteUIManager->SetUIAlpha(0.0f);
        qteUIManager->SetActive(true);
        if (CombatUIManager* combatUIManager = SingletonComponent<CombatUIManager>::GetInstance())
        {
            combatUIManager->SetActiveUI(!input);
        }
    }
}

void PlayerPlayTurnState::UpdateQuickTimeEventUI(float dt)
{
}

void PlayerPlayTurnState::UpdateAttackEventUI(float dt)
{
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49");
    {
        _inputState = InputState::NONE;
    }
    ImGui::End();
}

void PlayerPlayTurnState::SetAttackReady()
{
    Player& player = GetPlayer();
    auto* animator = player.GetAnimationComponent();
    if (animator)
    {
        // 오버라이드 애니메이션 빌드 시작
        animator->BeginBuildOverrideAnimation();
        animator->ClearOverrideAnimations();

        // 애니메이션 "Attack_Ready_Loop" USE_BLEND 및 USE_LOOP 플래그 설정 후  Push
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_USE_LOOP);
        animator->PushBackOverrideAnimation("Attack_Ready_Loop");
        
        // 애니메이션 "Attack_Ready" USE_BLEND 및 ALWAYS_UPDATE 플래그 설정 후 Push
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_ALWAYS_UPDATE);
        animator->PushBackOverrideAnimation("Attack_Ready");
        animator->SetCurrentAnimationPopCondition([](const AnimationData& data) { return data.IsEnd(); });  // 애니메이션이 끝날 경우 Pop

        // 애니메이션 빌드 종료
        animator->EndBuildOverrideAnimation();
    }
}

void PlayerPlayTurnState::SetAttack()
{
    // 애니메이션 처리
    Player& player   = GetPlayer();
    auto*   animator = player.GetAnimationComponent();
    auto*   audioTable = player.GetAudioTableComponent();
    if (animator)
    {
        animator->BeginBuildOverrideAnimation();
        animator->ClearOverrideAnimations();
    
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_USE_LOOP);
        animator->PushBackOverrideAnimation("Attack_Loop");
        
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_ALWAYS_UPDATE);
        animator->PushBackOverrideAnimation("Attack");
        animator->SetCurrentAnimationPopCondition([](const AnimationData& data) { return data.IsEnd(); }); // 애니메이션이 끝날 경우 Pop
        animator->SetCurrentAnimationPopCallback([this]() { OnQTEFinish(); });
    
        animator->EndBuildOverrideAnimation();
    }
}

void PlayerPlayTurnState::SetAttackEnd()
{
    bool succeed = false;
    Player& player   = GetPlayer();
    auto*   animator = player.GetAnimationComponent();
    if (animator)
    {
        animator->BeginBuildOverrideAnimation();
        animator->ClearOverrideAnimations();

        // 메인 애니메이션을 Idle로 바꿈
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_USE_LOOP | ANIMATION_FLAG_RESET_FRAME);
        animator->ChangeMainAnimation("Idle");

        // 애니메이션 "Attack_End"를 Push
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_ALWAYS_UPDATE);
        succeed = animator->PushBackOverrideAnimation("Attack_End");
        if (succeed)
        {   
            animator->SetCurrentAnimationPopCondition([](const AnimationData& data) { return data.IsEnd(); }); // 애니메이션이 끝날 경우 Pop
            animator->SetCurrentAnimationPopCallback([this]() { GetPlayer().EndTurn(); }); // Pop시 턴 종료
        }
        animator->EndBuildOverrideAnimation();
    }
    if (false == succeed)
    {
        player.EndTurn();
    }

    auto camera = dynamic_cast<UmCineMotion*>(CameraComponent::MainCamera());
    if (camera)
    {
        camera->StartRail(true);
    }
}

void PlayerPlayTurnState::BattleOnHitEvent(const QTE::NoteResult& result) 
{
    Battle::EnemyTargetFlag_ target = GetAttackTargetFromButton(result.PressedButton);
    Player& player = GetPlayer();
    Battle()(player, target, result);
    switch (result.Result)
    {
    case QTE::QTE_RESULT_PERFECT: {
        UmAudio.Play("-31000");
        break;
    }
    case QTE::QTE_RESULT_NORMAL: {
        UmAudio.Play("-31010");
        break;
    }
    case QTE::QTE_RESULT_MISS: {
        //UmAudio.Play("-31020");
        break;
    }
    default:
        break;
    }
}

Battle::EnemyTargetFlag_ PlayerPlayTurnState::GetAttackTargetFromButton(unsigned int button) const
{
    Battle::EnemyTargetFlag_ target;
    switch (button)
    {
    case Input::Controller::Button::X:
        target = Battle::EnemyTargetFlag_::ENEMY_TARGET_FLAG_LEFT;
        break;
    case Input::Controller::Button::Y:
        target = Battle::EnemyTargetFlag_::ENEMY_TARGET_FLAG_MIDDLE;
        break;
    case Input::Controller::Button::B:
        target = Battle::EnemyTargetFlag_::ENEMY_TARGET_FLAG_RIGHT;
        break;
    default: // 없으면 LEFT로 기본 설정
        target = Battle::EnemyTargetFlag_::ENEMY_TARGET_FLAG_LEFT;
        break;
    }
    return target;
}

void PlayerPlayTurnState::OnQTEFinish()
{
    float min = 0.0f; // 첫 무기 공격을 0초로 맞추고 나머지도 당겨오기 위한 변수
    const float offset = 0.3f;

    QTESystem* qteSystem = SingletonComponent<QTESystem>::GetInstance();
    if (qteSystem)
    {
        const QTE::OverallResult& results = qteSystem->GetQTEOverallResult();
        const auto& noteResults = results.NoteResults;
        for (const auto& result : noteResults)
        {
            float qteDelay  = qteSystem->GetDelayFromQTEStart();
            const QTE::NoteData* note = result.NoteData;
            if (note && result.IsValidResult())
            {
                bool  validModel = false;
                bool  validAnim  = false;
                bool  validEvent = false;
                float noteTime   = note->Time;

                WeaponModelManager* weaponModelManager = SingletonComponent<WeaponModelManager>::GetInstance();
                WeaponSystem*       weaponSystem       = SingletonComponent<WeaponSystem>::GetInstance();

                if (weaponModelManager && weaponSystem)
                {
                    const WeaponStats& weaponStats = weaponSystem->GetCurrentWeaponStats();
                    WeaponType         weaponType  = weaponStats.Type;

                    auto modelData = weaponModelManager->RequestAvailableWeapon(weaponType);
                    if (validModel = modelData.IsValid())
                    {
                        std::weak_ptr<GameObject> objectWeak = modelData.GameObject->GetWeakPtr();
                        AnimationComponent*       animation  = modelData.Animation;
                        ParticleComponent*        particle   = modelData.Particle;

                        modelData.GameObject->ActiveSelf = false;

                        // 노트에 맞는 애니메이션 설정 및 애니메이션 종료 콜백 등록
                        if (validAnim = modelData.Animation->ChangeMainAnimation(note->WeaponAnimationKey))
                        {
                            ++_attackRemaining;
                            animation->StopCurrentAnimation();
                            animation->SetMainAnimationEndCallback([this, objectWeak, weaponModelManager, modelData]() {
                                if (false == objectWeak.expired())
                                {
                                    if (modelData.IsValid())
                                    {
                                        modelData.Animation->StopCurrentAnimation();
                                        modelData.Particle->StopEffect("weapon");
                                    }
                                    weaponModelManager->ReturnWeaponModel(modelData);
                                    --_attackRemaining;
                                    if (0 >= _attackRemaining)
                                    {
                                        SetAttackEnd();
                                    }
                                }
                            });
                            animation->SetAnimationPostEventCallback(
                                [this, objectWeak, &result, modelData](const Timeline::EventContext* context) {
                                    if (false == objectWeak.expired() && modelData.IsValid())
                                    {
                                        if (context->GetLabel() == "Hit")
                                        {
                                            BattleOnHitEvent(result);
                                        }
                                    }
                                });

                            // 애니메이션 Hit 이벤트 콜백 등록
                            auto& animName  = modelData.Animation->GetAnimationNameFromKey(note->WeaponAnimationKey);
                            auto& animTrack = modelData.Animation->GetAnimationEventTrack();
                            auto  track     = animTrack.GetEventTrack(animName);
                            if (track)
                            {
                                if (Timeline::EventContext* context = track->GetContextFromLabel("Hit"))
                                {
                                    validEvent      = true;
                                    float hitTime   = context->Time;
                                    float noteDelay = note->WeaponAnimationDelay;
                                    float delta     = noteTime - hitTime + noteDelay + qteDelay - min;
                                    if (0.0f == min)
                                    {
                                        min   = delta;
                                        delta  = 0.0f;
                                    }
                                    delta += offset;
                                    
                                    UmTime.Invoke(delta, [objectWeak, animation, particle]() {
                                        if (auto object = objectWeak.lock())
                                        {
                                            object->ActiveSelf = true;
                                            if (animation)
                                            {
                                                animation->PlayCurrentAnimation();
                                            }
                                            if (particle)
                                            {
                                                particle->PlayEffect("weapon");
                                            }
                                        }
                                    });
                                }
                            }

                            // 무기 모델의 위치 설정
                            Battle::EnemyTargetFlag_ target  = GetAttackTargetFromButton(result.PressedButton);
                            auto                     enemies = Battle::GetTargetsFromFlags(target);
                            if (false == enemies.empty())
                            {
                                Enemy* enemy = enemies.front();
                                if (enemy)
                                {
                                    if (GameObject& player = GetPlayer().gameObject)
                                    {
                                        Vector3 enemyPos  = enemy->transform->GetWorldPosition();
                                        Vector3 playerPos = player.transform->GetWorldPosition();
                                        Vector3 dir       = DirectX::XMVector3Normalize(playerPos - enemyPos);

                                        const Vector3 offset   = weaponModelManager->GetWeaponOffset(weaponType);
                                        const Vector3 distance = offset * dir;
                                        modelData.GameObject->transform->SetWorldPosition(enemyPos + distance);
                                    }
                                }
                            }
                        }
                    }
                    if (false == validAnim)
                    {
                        weaponModelManager->ReturnWeaponModel(modelData);
                        --_attackRemaining;
                    }
                }
                if (false == validEvent)
                {   // 애니메이션이 없을 경우 바로 공격 처리
                    BattleOnHitEvent(result);
                }
            }
            else
            {
                // 노트가 없거나 판정이 유효하지 않을 경우 바로 공격 처리
                BattleOnHitEvent(result);
            }
        }
    }
    if (0 >= _attackRemaining)
    { // 모종의 이유로 공격이 하나도 없을 경우에 교착상태를 방지하기 위한 처리
        SetAttackEnd();
    }
}
