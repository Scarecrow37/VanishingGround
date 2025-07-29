#include "pchScripts.h"
#include "PlayerPlayTurnState.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <WeaponSystem/WeaponSystem.h>
#include <Animation/AnimationComponent.h>

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

    auto& player = GetPlayer();
    player.SetMainAnimation(CharacterBase::IDLE, ANIMATION_FLAG_USE_LOOP | ANIMATION_FLAG_RESET_FRAME);
}

void PlayerPlayTurnState::OnExit() 
{
    _inputState = InputState::NONE;
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
            SetAttackReadyAnimation();
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
            ReflectHelper::ImGuiDraw::Private::InputAuto(weapon.Name, UmCore->ImGuiDrawPropertysSetting);
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
                SetAttackAnimation();
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
        TurnMode* turnMode = TurnMode::GetInstance();
        if (turnMode)
        {
            float   delay  = 0.5f;
            Player& player = GetPlayer();
            for (auto& target : _attackTargets)
            {
                UmTime.Invoke(&GetFSM(), delay, [&player, target]() { Battle()(player, target); });
                delay += 0.5f;
            }
            _attackTargets.clear();
            _inputState = InputState::NONE;
            UmTime.Invoke(&GetFSM(), delay, [&]()
            {
                auto& player = GetPlayer();
                SetAttackEndAnimation();
            });
           
        }
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

void PlayerPlayTurnState::SetAttackReadyAnimation()
{
    Player& player = GetPlayer();
    auto* renderer = player.GetAnimationComponent();
    if (renderer)
    {
        renderer->BeginBuildOverrideAnimation();
        renderer->ClearOverrideAnimations();
        {
            const char* animKey = player.GetAnimationName(CharacterBase::ATTACK_READY_LOOP);
            renderer->PushOverrideAnimation(animKey, true);
            renderer->ChangeCurrentAnimationFlags(ANIMATION_FLAG_USE_LOOP);
        }
        {

            const char* animKey = player.GetAnimationName(CharacterBase::ATTACK_READY);
            renderer->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd(); });
            renderer->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
        }
        renderer->EndBuildOverrideAnimation();
    }
}

void PlayerPlayTurnState::SetAttackAnimation()
{
    Player& player   = GetPlayer();
    auto*   renderer = player.GetAnimationComponent();
    if (renderer)
    {
        renderer->BeginBuildOverrideAnimation();
        renderer->ClearOverrideAnimations();
        {
            const char* animKey = player.GetAnimationName(CharacterBase::ATTACK_LOOP);
            renderer->PushOverrideAnimation(animKey, true);
            renderer->ChangeCurrentAnimationFlags(ANIMATION_FLAG_USE_LOOP);
        }
        {
            const char* animKey = player.GetAnimationName(CharacterBase::ATTACK);
            renderer->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd(); });
            renderer->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
        }
        renderer->EndBuildOverrideAnimation();
    }
}

void PlayerPlayTurnState::SetAttackEndAnimation() 
{
    Player& player   = GetPlayer();
    auto*   renderer = player.GetAnimationComponent();
    if (renderer)
    {
        renderer->BeginBuildOverrideAnimation();
        renderer->ClearOverrideAnimations();
        {
            const char* animKey = player.GetAnimationName(CharacterBase::IDLE);
            renderer->ChangeMainAnimation(animKey);
            renderer->ChangeMainAnimationFlags(ANIMATION_FLAG_USE_LOOP | ANIMATION_FLAG_RESET_FRAME);
        }
        {
            const char*   animKey = player.GetAnimationName(CharacterBase::ATTACK_END);
            renderer->PushOverrideAnimation(animKey, true, [](const AnimationData& data) { return data.IsEnd(); });
            renderer->ChangeCurrentAnimationFlags(ANIMATION_FLAG_ALWAYS_UPDATE);
            renderer->SetCurrentAnimationPopCallback([this]() {
                // 애니메이션이 끝날 시 턴 종료
                auto& player = GetPlayer();
                player.EndTurn();
                });
        }
        renderer->EndBuildOverrideAnimation();
    }
}