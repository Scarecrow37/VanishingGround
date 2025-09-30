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


using namespace u8_literals;

REGISTER_CLASS(FSMStateFactory, PlayerPlayTurnState)

PlayerPlayTurnState::PlayerPlayTurnState() 
{
    _setImguiPosCenter        = false;
    _inputState               = InputState::NONE;
    _attackButtonHeldTime     = 0.f;
    _attackButtonHeldWaitTime = 2.0f;
    _attackRemaining          = 0;
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
    _setImguiPosCenter    = true;
    _attackButtonHeldTime = 0;
    _attackRemaining      = 0;

    if (QTEUIManager* qteUIManager = QTEUIManager::GetInstance())
    {
        qteUIManager->Refresh();
    }
}

void PlayerPlayTurnState::OnExit() 
{
    _inputState = InputState::NONE;
    _attackRemaining        = 0;
    _attackButtonHeldTime   = 0.0f;
    _isDownAButton          = false;
}


void PlayerPlayTurnState::OnUpdate() 
{
    if (true == _setImguiPosCenter)
    {
        ImGuiViewport* viewport  = ImGui::GetMainViewport();
        ImVec2         centerPos = ImVec2(viewport->Pos.x + (viewport->Size.x - 500.0f) * 0.5f,
                                          viewport->Pos.y + (viewport->Size.y - 500.0f) * 0.5f);

        ImGui::SetNextWindowPos(centerPos);
        _setImguiPosCenter = false;
    }

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
            qteSystem->StartQTE([this](const QTE::OverallResult& results) { OnQTEFinish(results); });
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
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49", nullptr, flags);
    {
#ifdef _UMEDITOR

        _isDownAKey = ImGui::IsKeyDown(ImGuiKey_A);
#endif // ISEDITOR

        auto enemies = Battle::GetTargetsFromFlags(Battle::ENEMY_TARGET_FLAG_ALL);
        if (ImGui::Button((const char*)u8"[적] 전멸"))
        {
            for (auto& enemy : enemies)
            {
                if (enemy)
                {
                    enemy->Dead();
                }
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
    }
    ImGui::End();
    ImGui::PopStyleColor();

    float t = _attackButtonHeldTime / _attackButtonHeldWaitTime;
    QTESystem*    qteSystem    = SingletonComponent<QTESystem>::GetInstance();
    QTEUIManager* qteUIManager = QTEUIManager::GetInstance();
    if (qteSystem && qteUIManager)
    {
        bool input = _isDownAKey || _isDownAButton;
        qteSystem->CombatUIActive(!input);
        qteUIManager->SetBackgroundUIAlpha(t);
        qteUIManager->SetUIAlpha(0.0f);
        qteUIManager->SetActive(true);
    }
}

void PlayerPlayTurnState::UpdateQuickTimeEventUI(float dt)
{
    //ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
    //ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
    //ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49", nullptr, flags);
    //{
    //    WeaponSystem* weaponSystem = SingletonComponent<WeaponSystem>::GetInstance();
    //    TurnMode*     turnMode     = SingletonComponent<TurnMode>::GetInstance();
    //    if (weaponSystem)
    //    {
    //        Player&      player = GetPlayer();
    //        WeaponStats& weapon = const_cast<WeaponStats&>(weaponSystem->GetCurrentWeaponStats());
    //        ImGui::BeginDisabled();
    //        ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.WeaponName, UmCore->ImGuiDrawPropertysSetting);
    //        ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.Type, UmCore->ImGuiDrawPropertysSetting);
    //        ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.HitDamage, UmCore->ImGuiDrawPropertysSetting);
    //        ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.CriticalDamage, UmCore->ImGuiDrawPropertysSetting);
    //        ImGui::EndDisabled();
    //        ImGui::Separator();
    //
    //        ImGui::Text((const char*)u8"X, Y, B를 눌러 공격하세요.");
    //        ImGui::Text((const char*)u8"남은 공격 횟수 : %d", _attackRemaining);
    //        int index = 0;
    //        for (auto& [name, value] : Battle::ENEMY_TARGET_FLAGS)
    //        {
    //            if (ImGui::Button(name))
    //            {
    //                PushAttackTarget(value);
    //            }      
    //            constexpr int lastIndex = std::size(Battle::ENEMY_TARGET_FLAGS) - 1;
    //            if (index < lastIndex)
    //            {
    //                ImGui::SameLine();
    //            }
    //            index++;
    //        }
    //
    //        ImGui::Separator();
    //        if (ImGui::Button((const char*)u8"[테스트] 자해"))
    //        {
    //            player.TakeDamage(10);
    //        }
    //        ImGui::SameLine();
    //        if (ImGui::Button((const char*)u8"[테스트] 자살"))
    //        {
    //            player.Dead();
    //        }
    //        ImGui::SameLine();
    //        if (ImGui::Button((const char*)u8"[테스트] 턴 종료"))
    //        {
    //            _attackRemaining = 0;
    //        }
    //        for (auto& target : _attackTargets)
    //        {
    //            ImGui::Text(Battle::EnemyTargetFlagToString(target).data());
    //        }
    //
    //        if (_attackRemaining == 0)
    //        {
    //            _inputState = InputState::ATTACK_EVENT;
    //            if (turnMode)
    //            {
    //                turnMode->ApplyActions([&player](TurnAction& action) { action.OnPlayerQTEResult(player); });
    //            }
    //            SetAttack();
    //        }
    //    }
    //    else
    //    {
    //        UmLogger.Message(LogLevel::LEVEL_DEBUG, u8"Weapon System이 존재하지 않습니다.");
    //    }
    //}
    //ImGui::End();
    //ImGui::PopStyleColor();
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
    
        animator->EndBuildOverrideAnimation();
    }
}

void PlayerPlayTurnState::SetAttackEnd()
{
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
        bool pushResult = animator->PushBackOverrideAnimation("Attack_End");
        if (pushResult)
        {   
            animator->SetCurrentAnimationPopCondition([](const AnimationData& data) { return data.IsEnd(); }); // 애니메이션이 끝날 경우 Pop
            animator->SetCurrentAnimationPopCallback([this]() { GetPlayer().EndTurn(); }); // Pop시 턴 종료
        }
        else
        {
            // 애니메이션을 못넣었으면 바로 턴 종료
            player.EndTurn();
        }

        animator->EndBuildOverrideAnimation();
    }
    else
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

void PlayerPlayTurnState::OnQTEFinish(const QTE::OverallResult& results)
{
    const auto& noteResults = results.NoteResults;
    for (const auto& result : noteResults)
    {   

        QTE::Note* note = result.Note;
        if (result.IsValidResult() && note)
        {
            bool  pushedAnimation = false;
            float noteTime = note->Time;
            //_attackTargets.push_back(target);
            WeaponModelManager* weaponModelManager = SingletonComponent<WeaponModelManager>::GetInstance();
            WeaponSystem*       weaponSystem       = SingletonComponent<WeaponSystem>::GetInstance();

            if (weaponModelManager && weaponSystem)
            {
                const WeaponStats& weaponStats = weaponSystem->GetCurrentWeaponStats();
                WeaponType         weaponType  = weaponStats.Type;

                auto modelData = weaponModelManager->RequestAvailableWeapon(weaponType);
                if (modelData.IsValid())
                {
                    modelData.GameObject->ActiveSelf = false;

                    // 노트에 맞는 애니메이션 설정 및 애니메이션 종료 콜백 등록
                    if (modelData.Animation->ChangeMainAnimation(note->WeaponAnimation))
                    {
                        modelData.Animation->StopCurrentAnimation();
                        modelData.Animation->SetMainAnimationEndCallback([this, weaponModelManager, modelData]() {
                            if (modelData.IsValid())
                            {
                                modelData.Animation->StopCurrentAnimation();
                                int id = modelData.GameObject->GetInstanceID();
                                UmParticleManager->StopEffect(modelData.Particle, "weapon");
                                modelData.GameObject->ActiveSelf = false;
                            }
                            weaponModelManager->ReturnWeaponModel(modelData);
                            --_attackRemaining;
                            if (0 >= _attackRemaining)
                            {
                                SetAttackEnd();
                            }
                        });
                        modelData.Animation->SetAnimationPostEventCallback(
                            [this, &result, modelData](const Timeline::EventContext* context) {
                                if (modelData.IsValid())
                                {
                                    if (context->GetLabel() == "Hit")
                                    {
                                        BattleOnHitEvent(result);
                                    }
                                }
                            });

                        // 애니메이션 Hit 이벤트 콜백 등록
                        auto& animName  = modelData.Animation->GetAnimationNameFromKey(note->WeaponAnimation);
                        auto& animTrack = modelData.Animation->GetAnimationEventTrack();
                        auto  track     = animTrack.GetEventTrack(animName);
                        if (track)
                        {
                            if (Timeline::EventContext* context = track->GetContextFromLabel("Hit"))
                            {
                                pushedAnimation = true;
                                float hitTime   = context->Time;
                                float delay     = note->GetWeaponAnimationDelay();
                                float delta     = noteTime - hitTime + delay;
                                UmTime.Invoke(delta, [this, modelData]() {
                                    if (modelData.IsValid())
                                    {
                                        modelData.GameObject->ActiveSelf = true;
                                        modelData.Animation->PlayCurrentAnimation();
                                        modelData.Particle->FollowBoneMatrix("weapon");
                                        UmParticleManager->PlayEffect(modelData.Particle, "weapon");
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

                                    const Vector3 distance = dir * 1.0f;
                                    modelData.GameObject->transform->SetWorldPosition(enemyPos + distance);

                                    // modelData.GameObject->transform->LookAt(playerPos);
                                }
                            }
                        }
                    }
                    else
                    {  
                        // 애니메이션이 없을 경우 바로 모델 반납
                        weaponModelManager->ReturnWeaponModel(modelData);
                    }
                }
            }
            if (pushedAnimation)
            {
                // 애니메이션이 성공적으로 푸쉬 되었을 경우 공격 남은 횟수 증가
                ++_attackRemaining;
            }
            else
            {
                // 애니메이션이 없을 경우 바로 공격 처리
                BattleOnHitEvent(result);
            }
        }
    }
    if (0 < _attackRemaining)
    {   // 공격이 하나라도 있을 경우에 공격 애니메이션 실행
        SetAttack();
    }
    else
    {   // 모종의 이유로 공격이 하나도 없을 경우에 교착상태를 방지하기 위한 처리
        SetAttackEnd();
    }
}
