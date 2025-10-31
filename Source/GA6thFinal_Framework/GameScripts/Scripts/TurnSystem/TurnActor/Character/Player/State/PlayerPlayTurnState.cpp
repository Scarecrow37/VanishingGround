#include "pchScripts.h"
#include "PlayerPlayTurnState.h"
#include "Utility/FocusHelper.h"

#include <Animation/AnimationComponent.h>
#include <Particle/ParticleComponent.h>
#include <GameCore/FSM/FiniteStateMachine.h>
#include <Camera/UmCineMotion.h>

#include <QTE/System/QTESystem.h>
#include <QTE/UI/QTEUIManager.h>
#include <QTE/Track/QTETrack.h>
#include <WeaponSystem/WeaponSystem.h>
#include <WeaponModel/WeaponModelManager.h>

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnMode/State/PlayerActionPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

#include <TutorialSystem/TutorialSystem.h>

#include <CombatUIManager/CombatUIManager.h>

#include <Stats/CharacterStats.h>

using namespace u8_literals;

REGISTER_CLASS(FSMStateFactory, PlayerPlayTurnState)

PlayerPlayTurnState::PlayerPlayTurnState() 
{
    _inputState               = InputState::NONE;
    _attackButtonHeldTime     = 0.f;
    _attackButtonHeldWaitTime = 0.5f;
    _isDownAButton            = false;
    _isDownAKey               = false;
    _qteCallbackHandle        = 0;
}

PlayerPlayTurnState::~PlayerPlayTurnState() 
{
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        system->UnRegisterCallback(_qteCallbackHandle);
    }
}

void PlayerPlayTurnState::OnAwake() 
{
    BindInputAction(ControllerButton::A, Action::PRESSED, &GetFSM(), this, &PlayerPlayTurnState::PressedButtonA);
    BindInputAction(ControllerButton::A, Action::RELEASED, &GetFSM(), this, &PlayerPlayTurnState::ReleasedButtonA);
}

void PlayerPlayTurnState::OnStart() 
{
    if (QTESystem* system = SingletonComponent<QTESystem>::GetInstance())
    {
        QTE::Callback callback(GetPlayer().GetWeakPtr());
        // owner의 weak_ptr을 들고있으므로 람다 인자로 weak_ptr넣을 필요 X
        callback.OnFadeOutFinish = [this](const QTE::OverallResult& results) { SetAttack(); };
        _qteCallbackHandle = system->RegisterCallback(callback);
    }
}

void PlayerPlayTurnState::OnEnter() 
{
    _inputState           = InputState::ACTION_SELECTION;
    _attackButtonHeldTime = 0;

    WeaponSystem* system = SingletonComponent<WeaponSystem>::GetInstance();
    if (system)
    {
        const std::string& weaponName = system->GetCurrentWeaponElement().Stats.WeaponName;
        std::string        message = std::format("{}{}{}", (const char*)u8"Player 턴 시작. ", "Weapon : ", weaponName);
        UmLogger.Message(LogLevel::LEVEL_TRACE, message);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8" WeaponSystem이 존재하지 않습니다.");
    }
}

void PlayerPlayTurnState::OnExit() 
{
    _inputState             = InputState::NONE;
    _attackButtonHeldTime   = 0.0f;
    _isDownAButton          = false;

    if (TurnMode* mode = SingletonComponent<TurnMode>::GetInstance())
    {
        mode->ApplyActions([this](TurnAction& action) 
        { 
            action.OnTurnEnd(GetPlayer());
        });
    }

    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"Player 턴 종료.");
}


void PlayerPlayTurnState::OnUpdate() 
{
    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        if (PlayerActionPhase* waitPhase = turnMode->States->PlayerActionPhase)
        {
            if (false == waitPhase->WaitPhase)
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
                    // UpdateQuickTimeEventUI(dt);
                    break;
                case InputState::ATTACK_EVENT:
                    UpdateAttackEventUI(dt);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void PlayerPlayTurnState::PressedButtonA(const Input::Controller& controller)
{
    if (false == FocusHelper::CheckFocus(FocusHelper::IS_FOCUSED_ANYTHING))
    {
        _isDownAButton = true;
        if (InputState::ACTION_SELECTION == _inputState)
        {
            if (CombatUIManager* uiManager = SingletonComponent<CombatUIManager>::GetInstance())
                uiManager->FadeOut(_attackButtonHeldWaitTime);
        } 
    }
}

void PlayerPlayTurnState::ReleasedButtonA(const Input::Controller& controller)
{
    _isDownAButton = false;
    if (InputState::ACTION_SELECTION == _inputState)
    {
        if (CombatUIManager* uiManager = SingletonComponent<CombatUIManager>::GetInstance())
            uiManager->FadeIn(_attackButtonHeldWaitTime);
    }  
}

void PlayerPlayTurnState::UpdateAttackButtonHeld(float dt)
{
    if (_attackButtonHeldTime >= _attackButtonHeldWaitTime)
    {
        if (QTESystem* qteSystem = SingletonComponent<QTESystem>::GetInstance())
        {
            _inputState = InputState::QUICK_TIME_EVENT;
            qteSystem->StartQTE();
            SetAttackReady();
            if (TutorialSystem* system = SingletonComponent<TutorialSystem>::GetInstance())
            {
                system->Show(805907);
            }
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
        if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
        {
            auto& enemies = turnMode->GetEnemies();

            if (ImGui::Button((const char*)u8"적 전멸"))
            {
                for (auto& enemy : enemies)
                {
                    if (enemy)
                        enemy->Dead();
                }
            }
            for (size_t i = 0; i < enemies.size(); ++i)
            {
                ImGui::SameLine();
                const char* spawnPointStr = Monster::SpawnPointToString(enemies[i]->SpawnPoint);
                std::string buttonLabel   = std::format("{}{}", spawnPointStr, (const char*)u8" 적 자살");
                if (ImGui::Button(buttonLabel.c_str()))
                {
                    if (enemies[i])
                    {
                        enemies[i]->Dead();
                    }
                }
                ImGui::SameLine();
            }
        }
        ImGui::Separator();
        Player& player = GetPlayer();
        if (ImGui::Button((const char*)u8"[플레이어] 회복"))
        {
            if (CharacterStats* stats = player.GetCharacterStats())
            {
                stats->CurrentHP += 10;
                stats->CurrentHP = std::clamp((int)stats->CurrentHP, 0, (int)stats->MaxHP);
            }
        }
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

    const bool input = _isDownAKey || _isDownAButton;
    const float t    = _attackButtonHeldTime / _attackButtonHeldWaitTime;
    if (QTEUIManager* qteUIManager = SingletonComponent<QTEUIManager>::GetInstance())
    {
        qteUIManager->SetUIAlpha(t);
    }
}

void PlayerPlayTurnState::UpdateQuickTimeEventUI(float dt)
{
}

void PlayerPlayTurnState::UpdateAttackEventUI(float dt)
{
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
    //UmAudio.Play("-32000");
}

void PlayerPlayTurnState::SetAttack()
{
    // 애니메이션 처리
    Player& player   = GetPlayer();
    auto*   animator = player.GetAnimationComponent();
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
    //UmAudio.Play("-32010");
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

void PlayerPlayTurnState::BattleOnHitEvent(QTE::NoteResult& result)
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
    _inputState = InputState::ATTACK_EVENT;

    if (CombatUIManager* uiManager = SingletonComponent<CombatUIManager>::GetInstance())
        uiManager->FadeIn(_attackButtonHeldWaitTime);

    float animOffset = 0.0f; // 첫 무기 공격을 0초로 맞추고 나머지도 당겨오기 위한 변수
    float totalTime = 0.0f;
    
    QTESystem*          qteSystem           = SingletonComponent<QTESystem>::GetInstance();
    WeaponSystem*       weaponSystem        = SingletonComponent<WeaponSystem>::GetInstance();
    WeaponModelManager* weaponModelManager  = SingletonComponent<WeaponModelManager>::GetInstance();

    assert(qteSystem            && "QTESystem이 없습니다.");
    assert(weaponSystem         && "WeaponSystem이 없습니다.");
    assert(weaponModelManager   && "WeaponModelManager가 없습니다.");

    if (qteSystem && weaponSystem && weaponModelManager)
    {
        QTE::OverallResult& results = qteSystem->GetQTEOverallResult();
        for (auto& result : results.NoteResults)
        {
            if (result.IsValidResult())
            {
                const WeaponStats& weaponStats = weaponSystem->GetCurrentWeaponElement().Stats;
                WeaponModelData    weaponModel = weaponModelManager->RequestAvailableWeapon(weaponStats.Type);
                if (weaponModel.IsValid())
                {
                    SetWeaponModelCallback(weaponModel, result);
                    SetWeaponModelTransform(weaponModel, result);

                    if (const QTE::NoteData* note = result.NoteData)
                    {
                        // 애니메이션 Hit 이벤트 콜백 등록
                        auto& animName  = weaponModel.Animation->GetAnimationNameFromKey(note->WeaponAnimationKey);
                        auto& animTrack = weaponModel.Animation->GetAnimationEventTrack();
                        if (auto track = animTrack.GetEventTrack(animName))
                        {
                            if (Timeline::EventContext* context = track->GetContextFromLabel("Hit"))
                            {
                                const float noteTime  = note->Time;
                                const float hitTime   = context->Time;
                                const float noteDelay = note->WeaponAnimationDelay;
                                float       delta     = noteTime - hitTime + noteDelay + animOffset;
                                // 0보다 낮으면 0초로 맞추고 나머지를 해당 오프셋만큼 이동
                                if (delta < 0)
                                {
                                    animOffset = -delta;
                                    delta      = 0.0f;
                                }
                                totalTime = delta + hitTime;

                                auto weakWeapon = weaponModel.GameObject->GetWeakPtr();
                                UmTime.Invoke(delta, [weakWeapon, weaponModel]() {
                                    if (auto object = weakWeapon.lock())
                                    {
                                        object->ActiveSelf = true;
                                        if (weaponModel.Animation)
                                        {
                                            weaponModel.Animation->PlayCurrentAnimation();
                                        }
                                        if (weaponModel.Particle)
                                        {
                                            weaponModel.Particle->PlayEffect("weapon");
                                        }
                                    }
                                });
                            }
                        }
                    }
                }
            }
        }
        SetAttackEndTimeInvoke(totalTime);
    }
    else
    {
        SetAttackEnd();
    }
}

void PlayerPlayTurnState::SetWeaponModelCallback(WeaponModelData& modelData, QTE::NoteResult& noteResult)
{
    bool validModel = false;
    bool validAnim  = false;

    WeaponModelManager* weaponModelManager = SingletonComponent<WeaponModelManager>::GetInstance();
    assert(weaponModelManager && "WeaponModel을 가져올 Manager가 없습니다.");
    if (weaponModelManager)
    {
        if (const QTE::NoteData* note = noteResult.NoteData)
        {
            if (validModel = modelData.IsValid())
            {
                std::weak_ptr<GameObject> objectWeak = modelData.GameObject->GetWeakPtr();
                AnimationComponent*       animation  = modelData.Animation;
                ParticleComponent*        particle   = modelData.Particle;

                modelData.GameObject->ActiveSelf = false;
                // 노트에 맞는 애니메이션 설정 및 애니메이션 종료 콜백 등록
                if (validAnim = modelData.Animation->ChangeMainAnimation(note->WeaponAnimationKey))
                {
                    animation->StopCurrentAnimation();
                    animation->SetMainAnimationEndCallback(
                            [this, objectWeak, weaponModelManager, modelData]() {
                            if (false == objectWeak.expired() && modelData.IsValid())
                            {
                                modelData.Animation->StopCurrentAnimation();
                                modelData.Particle->StopEffect("weapon");
                                weaponModelManager->ReturnWeaponModel(modelData);
                            }
                        });
                    animation->SetAnimationPostEventCallback(
                        [this, objectWeak, &noteResult, modelData](const Timeline::EventContext* context) {
                            if (false == objectWeak.expired() && modelData.IsValid())
                            {
                                if (context->GetLabel() == "Hit")
                                {
                                    BattleOnHitEvent(noteResult);
                                }
                            }
                        });
                }
            }
        }
    }
}

void PlayerPlayTurnState::SetWeaponModelTransform(WeaponModelData& modelData, QTE::NoteResult& noteResult)
{
    WeaponModelManager* weaponModelManager = SingletonComponent<WeaponModelManager>::GetInstance();
    assert(weaponModelManager && "WeaponModel을 가져올 Manager가 없습니다.");
    if (weaponModelManager)
    {
        // 무기 모델의 위치 설정
        Battle::EnemyTargetFlag_ target  = GetAttackTargetFromButton(noteResult.PressedButton);
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

                    const Vector3 offset   = weaponModelManager->GetWeaponOffset(modelData.Type);
                    const Vector3 distance = offset * dir;
                    modelData.GameObject->transform->SetWorldPosition(enemyPos + distance);
                }
            }
        }
    }
}

void PlayerPlayTurnState::SetAttackEndTimeInvoke(float time)
{
    std::weak_ptr<Component> weakOwner = GetPlayer().GetWeakPtr();
    UmTime.Invoke(time, [this, weakOwner]() {
        if (false == weakOwner.expired())
        {
            SetAttackEnd();
        }
    });
}
