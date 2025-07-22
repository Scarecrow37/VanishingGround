#include "pchScripts.h"
#include "PlayerPlayTurnState.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <WeaponSystem/WeaponSystem.h>

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <Mesh/SkeletalMeshRenderer.h>

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
    player.SetMainAnimation(CharacterBase::IDLE);
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
        ImGui::SetNextWindowSize(ImVec2(500, 500));

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
    PushAttackTarget(AttackTarget::LEFT);
}

void PlayerPlayTurnState::ReleasedButtonX(const Input::Controller& controller) 
{

}

void PlayerPlayTurnState::PressedButtonY(const Input::Controller& controller) 
{
    PushAttackTarget(AttackTarget::MIDDLE);
}

void PlayerPlayTurnState::ReleasedButtonY(const Input::Controller& controller) 
{

}

void PlayerPlayTurnState::PressedButtonB(const Input::Controller& controller) 
{
    PushAttackTarget(AttackTarget::RIGHT);
}

void PlayerPlayTurnState::ReleasedButtonB(const Input::Controller& controller) 
{

}

void PlayerPlayTurnState::UpdateActionSelectionUI(float dt) 
{
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49");
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
}

void PlayerPlayTurnState::UpdateQuickTimeEventUI(float dt)
{
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49");
    {
        WeaponSystem* weaponSystem = WeaponSystem::GetInstance();
        if (weaponSystem)
        {
            Player&      player = GetPlayer();
            WeaponStats& weapon = const_cast<WeaponStats&>(weaponSystem->GetCurrentWeaponStats());
            ImGui::BeginDisabled();
            weapon.ImGuiDrawPropertys();
            ImGui::EndDisabled();
            ImGui::Separator();

            ImGui::Text((const char*)u8"X, Y, B를 눌러 공격하세요.");
            ImGui::Text((const char*)u8"남은 공격 횟수 : %d", _attackRemaining);
            constexpr auto targets = rfl::get_enumerator_array<AttackTarget>();
            for (auto& [name, value] : targets)
            {
                if (ImGui::Button(name.data()))
                {
                    PushAttackTarget(value);
                }
            }

            for (auto& target : _attackTargets)
            {
                ImGui::Text(rfl::enum_to_string(target).c_str());
            }

            if (_attackRemaining == 0)
            {
                _inputState = InputState::ATTACK_EVENT;
                SetAttackAnimation();
            }
        }
        else
        {
            UmLogger.Message(LogLevel::LEVEL_DEBUG, u8"Weapon System이 존재하지 않습니다.");
        }
    }
    ImGui::End();
}

void PlayerPlayTurnState::UpdateAttackEventUI(float dt)
{
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49");
    {
        TurnMode* turnMode = TurnMode::GetInstance();
        if (turnMode)
        {
            auto& combatStartPhase = turnMode->States->CombatStartPhase;
            if (combatStartPhase)
            {
                float delay = 0.5f;
                Player& player = GetPlayer();
                const auto& enemys = combatStartPhase->GetEnemies();
                for (auto& target : _attackTargets)
                {
                    int targetIndex = static_cast<int>(target);
                    try
                    {
                        Enemy* enemy = enemys.at(targetIndex);
                        if (enemy)
                        {
                            UmTime.Invoke(&GetFSM(), delay, [&player, enemy]() { TurnMode::Battle()(player, *enemy); });                         
                            delay += 0.5f;
                        }
                    }
                    catch (const std::exception&)
                    {
                        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"유효하지 않은 enemy Index 입니다.");
                    }
                }
                _attackTargets.clear();
                _inputState = InputState::NONE;
                UmTime.Invoke(&GetFSM(), delay, 
                [&]() 
                { 
                    auto& player = GetPlayer();
                    SetAttackEndAnimation();
                    player.EndTurn();
                });
            }
        }
    }
    ImGui::End();
}

bool PlayerPlayTurnState::IsAttackable() const
{
    return _inputState == InputState::QUICK_TIME_EVENT && 0 < _attackRemaining;
}

void PlayerPlayTurnState::PushAttackTarget(AttackTarget target)
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
    SkeletalMeshRenderer* renderer = player.GetSkeletalMeshRenderer();
    if (renderer)
    {
        renderer->BeginBuildOverrideAnimation();
        renderer->ClearOverrideAnimations();
        player.PushOverrideAnimation(CharacterBase::ATTACK_READY_LOOP);
        player.PushOverrideAnimation(CharacterBase::ATTACK_READY, false, true,
                                     [](const AnimationData& data) { return data.IsEnd; });
        renderer->EndBuildOverrideAnimation();
    }
}

void PlayerPlayTurnState::SetAttackAnimation()
{
    Player&               player   = GetPlayer();
    SkeletalMeshRenderer* renderer = player.GetSkeletalMeshRenderer();
    if (renderer)
    {
        renderer->BeginBuildOverrideAnimation();
        renderer->ClearOverrideAnimations();
        player.PushOverrideAnimation(CharacterBase::ATTACK_LOOP);
        player.PushOverrideAnimation(CharacterBase::ATTACK, false, true,
                                    [](const AnimationData& data) { return data.IsEnd; });
        renderer->EndBuildOverrideAnimation();
    }
}

void PlayerPlayTurnState::SetAttackEndAnimation() 
{
    Player&               player   = GetPlayer();
    SkeletalMeshRenderer* renderer = player.GetSkeletalMeshRenderer();
    if (renderer)
    {
        renderer->BeginBuildOverrideAnimation();
        renderer->ClearOverrideAnimations();
        player.SetMainAnimation(CharacterBase::IDLE);
        renderer->SetMainAnimationFrame(0.0f);
        player.PushOverrideAnimation(CharacterBase::ATTACK_END, false, true,
                                     [](const AnimationData& data) { return data.IsEnd; });
        renderer->EndBuildOverrideAnimation();
    }
}