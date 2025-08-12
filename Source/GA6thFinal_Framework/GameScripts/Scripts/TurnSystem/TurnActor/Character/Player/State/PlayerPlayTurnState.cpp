#include "pchScripts.h"
#include "PlayerPlayTurnState.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <WeaponSystem/WeaponSystem.h>
#include <Animation/AnimationComponent.h>
#include <Audio/Table/AudioTableComponent.h>
#include <Particle/ParticleComponent.h>

using namespace u8_literals;

REGISTER_CLASS(FSMStateFactory, PlayerPlayTurnState)

PlayerPlayTurnState::PlayerPlayTurnState() 
{
    _setImguiPosCenter        = false;
    _inputState               = InputState::NONE;
    _isDownAttackButton       = false;
    _attackButtonHeldTime     = 0.f;
    _attackButtonHeldWaitTime = 1.5f;
    _attackRemaining          = 0;
}

PlayerPlayTurnState::~PlayerPlayTurnState() 
{

}

void PlayerPlayTurnState::OnAwake() 
{
    BindInputAction(ControllerButton::A, Action::PRESSED, &GetFSM(), this, &PlayerPlayTurnState::PressedButtonA);
    BindInputAction(ControllerButton::A, Action::RELEASED, &GetFSM(), this, &PlayerPlayTurnState::ReleasedButtonA);

    BindInputAction(ControllerButton::B, Action::PRESSED, &GetFSM(), this, &PlayerPlayTurnState::PressedButtonB);
    BindInputAction(ControllerButton::B, Action::RELEASED, &GetFSM(), this, &PlayerPlayTurnState::ReleasedButtonB);

    BindInputAction(ControllerButton::X, Action::PRESSED, &GetFSM(), this, &PlayerPlayTurnState::PressedButtonX);
    BindInputAction(ControllerButton::X, Action::RELEASED, &GetFSM(), this, &PlayerPlayTurnState::ReleasedButtonX);

    BindInputAction(ControllerButton::Y, Action::PRESSED, &GetFSM(), this, &PlayerPlayTurnState::PressedButtonY);
    BindInputAction(ControllerButton::Y, Action::RELEASED, &GetFSM(), this, &PlayerPlayTurnState::ReleasedButtonY);

    auto weaponList = GameObject::FindGameObjectsWithTag("Weapon");
    for (const auto& weapon : weaponList)
    {
        auto sharedWeapon = weapon.lock();
        if (sharedWeapon)
        {
            WeaponType type = WeaponType::SWORD;
            if (sharedWeapon->CompareTag("Sword")) 
                type = WeaponType::SWORD;
            else if (sharedWeapon->CompareTag("Dagger")) 
                type = WeaponType::DAGGER;
            else if (sharedWeapon->CompareTag("Mace")) 
                type = WeaponType::WARHAMMER;
            weaponAnims[(int)type]   = sharedWeapon->GetComponent<AnimationComponent>();
            weaponEffects[(int)type] = sharedWeapon->GetComponent<ParticleComponent>();
            if (weaponAnims[(int)type] != nullptr)
            {
                weaponAnims[(int)type]->StopCurrentAnimation();
                weaponAnims[(int)type]->SetAnimationPostEventCallback([this](const Timeline::EventContext* context) {
                    const std::string& label = context->GetLabel();
                    if ("Attack" == label)
                    {
                        BattleOnAttackEvent();
                    }
                });
            }
            
        }
    }
}

void PlayerPlayTurnState::OnStart() 
{
}

void PlayerPlayTurnState::OnEnter() 
{
    _inputState           = InputState::ACTION_SELECTION;
    _isDownAttackButton   = false;
    _setImguiPosCenter    = true;
    _attackButtonHeldTime = 0;
    _attackRemaining      = 0;
}

void PlayerPlayTurnState::OnExit() 
{
    _inputState = InputState::NONE;
    _attackTargets.clear();
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
    case PlayerPlayTurnState::InputState::NONE:
        break;
    case PlayerPlayTurnState::InputState::ACTION_SELECTION:
        UpdateAttackButtonHeld(dt);
        UpdateActionSelectionUI(dt);
        break;
    case PlayerPlayTurnState::InputState::QUICK_TIME_EVENT:
        UpdateQuickTimeEventUI(dt);
        break;
    case PlayerPlayTurnState::InputState::ATTACK_EVENT:
        UpdateAttackEventUI(dt);
        break;
    default:
        break;
    }
}

void PlayerPlayTurnState::UpdateAttackButtonHeld(float dt) 
{
    if (_isDownAttackButton)
    {
        _attackButtonHeldTime += dt;
        if (_attackButtonHeldTime >= _attackButtonHeldWaitTime)
        {
            _inputState = PlayerPlayTurnState::InputState::QUICK_TIME_EVENT;
            WeaponSystem* weaponSystem = WeaponSystem::GetInstance();
            if (weaponSystem)
            {
                const WeaponStats& weapon = weaponSystem->GetCurrentWeaponStats();
                _attackRemaining = weapon.AttackCount;
                _setImguiPosCenter = true;
                _attackTargets.clear();
            }
            SetAttackReady();
        }
    }
}

void PlayerPlayTurnState::PressedButtonA(const Input::Controller& controller)
{
    if (_inputState == InputState::ACTION_SELECTION)
    {
        _isDownAttackButton = true;
    }

}

void PlayerPlayTurnState::ReleasedButtonA(const Input::Controller& controller) 
{
    if (_inputState == InputState::ACTION_SELECTION)
    {
        _attackButtonHeldTime = 0.f;
        _isDownAttackButton = false;
    }
}

void PlayerPlayTurnState::PressedButtonX(const Input::Controller& controller) 
{
    PushAttackTarget(Battle::ENEMY_TARGET_FLAG_LEFT);
}

void PlayerPlayTurnState::ReleasedButtonX(const Input::Controller& controller) 
{

}

void PlayerPlayTurnState::PressedButtonY(const Input::Controller& controller) 
{
    PushAttackTarget(Battle::ENEMY_TARGET_FLAG_MIDDLE);
}

void PlayerPlayTurnState::ReleasedButtonY(const Input::Controller& controller) 
{

}

void PlayerPlayTurnState::PressedButtonB(const Input::Controller& controller) 
{
    PushAttackTarget(Battle::ENEMY_TARGET_FLAG_RIGHT);
}

void PlayerPlayTurnState::ReleasedButtonB(const Input::Controller& controller) 
{

}

void PlayerPlayTurnState::UpdateActionSelectionUI(float dt) 
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49", nullptr, flags);
    {
        if (ImGui::Button((const char*)u8"A를 눌러 공격 진입"))
        {
            _attackButtonHeldTime = _attackButtonHeldWaitTime;
            _isDownAttackButton   = true;
        }
        float t = _attackButtonHeldTime / _attackButtonHeldWaitTime;
        ImGui::ProgressBar(t);
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

void PlayerPlayTurnState::UpdateQuickTimeEventUI(float dt)
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 0.5f));
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49", nullptr, flags);
    {
        WeaponSystem* weaponSystem = WeaponSystem::GetInstance();
        TurnMode*     turnMode     = TurnMode::GetInstance();
        if (weaponSystem)
        {
            Player&      player = GetPlayer();
            WeaponStats& weapon = const_cast<WeaponStats&>(weaponSystem->GetCurrentWeaponStats());
            ImGui::BeginDisabled();
            ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.WeaponName, UmCore->ImGuiDrawPropertysSetting);
            ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.Type, UmCore->ImGuiDrawPropertysSetting);
            ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.HitDamage, UmCore->ImGuiDrawPropertysSetting);
            ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.CriticalDamage, UmCore->ImGuiDrawPropertysSetting);
            ImGui::EndDisabled();
            ImGui::Separator();

            ImGui::Text((const char*)u8"X, Y, B를 눌러 공격하세요.");
            ImGui::Text((const char*)u8"남은 공격 횟수 : %d", _attackRemaining);
            int index = 0;
            for (auto& [name, value] : Battle::ENEMY_TARGET_FLAGS)
            {
                if (ImGui::Button(name))
                {
                    PushAttackTarget(value);
                }      
                constexpr int lastIndex = std::size(Battle::ENEMY_TARGET_FLAGS) - 1;
                if (index < lastIndex)
                {
                    ImGui::SameLine();
                }
                index++;
            }

            ImGui::Separator();
            if (ImGui::Button((const char*)u8"[테스트] 자해"))
            {
                player.TakeDamage(10);
            }
            ImGui::SameLine();
            if (ImGui::Button((const char*)u8"[테스트] 자살"))
            {
                player.Dead();
            }
            ImGui::SameLine();
            if (ImGui::Button((const char*)u8"[테스트] 턴 종료"))
            {
                _attackRemaining = 0;
            }
            for (auto& target : _attackTargets)
            {
                ImGui::Text(Battle::EnemyTargetFlagToString(target).data());
            }

            if (_attackRemaining == 0)
            {
                _inputState = InputState::ATTACK_EVENT;
                if (turnMode)
                {
                    turnMode->ApplyActions([&player](TurnAction& action) { action.OnPlayerQTEResult(player); });
                }
                SetAttack();
            }
        }
        else
        {
            UmLogger.Message(LogLevel::LEVEL_DEBUG, u8"Weapon System이 존재하지 않습니다.");
        }
    }
    ImGui::End();
    ImGui::PopStyleColor();
}

void PlayerPlayTurnState::UpdateAttackEventUI(float dt)
{
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49");
    {
        _inputState = InputState::NONE;
    }
    ImGui::End();
}

bool PlayerPlayTurnState::IsAttackable() const
{
    return _inputState == InputState::QUICK_TIME_EVENT && 0 < _attackRemaining;
}

void PlayerPlayTurnState::PushAttackTarget(Battle::EnemyTargetFlag_ target)
{
    if (IsAttackable())
    {
        _attackTargets.push_back(target);
        --_attackRemaining;
    }
}

void PlayerPlayTurnState::SetAttackReady()
{
    Player& player = GetPlayer();
    auto* animator = player.GetAnimationComponent();
    auto* audioTable = player.GetAudioTableComponent();
    if (animator)
    {
        animator->BeginBuildOverrideAnimation();
        animator->ClearOverrideAnimations();

        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_USE_LOOP);
        animator->PushBackOverrideAnimation("Attack_Ready_Loop");
        
        animator->SetNextAnimationFlags(ANIMATION_FLAG_USE_BLEND | ANIMATION_FLAG_ALWAYS_UPDATE);
        animator->PushBackOverrideAnimation("Attack_Ready");
        animator->SetCurrentAnimationPopCondition([](const AnimationData& data) { return data.IsEnd(); });  // 애니메이션이 끝날 경우 Pop

        animator->EndBuildOverrideAnimation();
    }

    WeaponSystem*       weaponSystem = WeaponSystem::GetInstance();
    const WeaponStats&  weaponStats  = weaponSystem->GetCurrentWeaponStats();
    WeaponType          weaponType   = weaponStats.Type;
    AnimationComponent* weaponAnim   = weaponAnims[(int)weaponType];
    ParticleComponent*  weaponEffect = weaponEffects[(int)weaponType];

    if (weaponAnim)
    {
        weaponAnim->BeginBuildOverrideAnimation();
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

    // 무기 애니메이션 및 이펙트 처리
    WeaponSystem*       weaponSystem = WeaponSystem::GetInstance();
    const WeaponStats&  weaponStats  = weaponSystem->GetCurrentWeaponStats();
    WeaponType          weaponType   = weaponStats.Type;
    AnimationComponent* weaponAnim   = weaponAnims[(int)weaponType];
    ParticleComponent*  weaponEffect = weaponEffects[(int)weaponType];

    // 무기 애니메이션 처리
    if (false == _attackTargets.empty())
    {
        // 무기 이펙트 처리
        if (weaponEffect)
        {
            weaponEffect->PlayEffect();
        }
        bool isFirst = true;
        for (auto iter = _attackTargets.rbegin(); iter != _attackTargets.rend(); ++iter)
        {
            // 임시 랜덤 애니메이션
            const auto& keymap = weaponAnim->GetAnimationKeyMap();
            int count = 0, randomIndex = Random::Range(0, (int)keymap.size() - 1);
            for (auto& [key, value] : keymap)
            {
                if (count == randomIndex)
                {   // 무기 애니메이션 설정(중복 Push 허용)
                    weaponAnim->PushBackOverrideAnimation(key, true);
                    weaponAnim->SetCurrentAnimationPopCondition([](const AnimationData& data) { return data.IsEnd(); }); // 애니메이션이 끝날 경우 Pop
                    if (isFirst)
                    {
                        weaponAnim->SetCurrentAnimationPopCallback([this]() { SetAttackEnd(); });
                        isFirst = false;
                    }
                }
                ++count;
            }
        }
        weaponAnim->PlayCurrentAnimation();
    }
    else
    {
        // 공격 대상이 없으면 애니메이션을 스킵
        SetAttackEnd();
    }
    if (weaponAnim)
    {
        weaponAnim->EndBuildOverrideAnimation();
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

    // 무기 애니메이션 및 이펙트 처리
    WeaponSystem*       weaponSystem = WeaponSystem::GetInstance();
    const WeaponStats&  weaponStats  = weaponSystem->GetCurrentWeaponStats();
    WeaponType          weaponType   = weaponStats.Type;
    AnimationComponent* weaponAnim   = weaponAnims[(int)weaponType];
    ParticleComponent*  weaponEffect = weaponEffects[(int)weaponType];

    if (weaponEffect)
    {
        weaponEffect->StopEffect();
    }
}

void PlayerPlayTurnState::BattleOnAttackEvent()
{
    if (!_attackTargets.empty())
    {
        auto&   target = _attackTargets.back();
        Player& player = GetPlayer();
        Battle()(player, target);
        _attackTargets.pop_back();
    }
}
