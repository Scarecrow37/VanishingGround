#include "pchScripts.h"
#include "PlayerPlayTurnState.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <WeaponSystem/WeaponSystem.h>

using namespace u8_literals;

REGISTER_CLASS(FSMStateFactory, PlayerPlayTurnState)

PlayerPlayTurnState::PlayerPlayTurnState() 
{
    _setImguiPosCenter        = false;
    _inputState               = InputState::NONE;
    _isDownAttackButton       = false;
    _attackButtonHeldTime     = 0.f;
    _attackButtonHeldWaitTime = 1.5f;
}

PlayerPlayTurnState::~PlayerPlayTurnState() 
{

}

void PlayerPlayTurnState::OnAwake() 
{
    BindInputAction(ControllerButton::A, Action::PRESSED, &GetFSM(), this, &PlayerPlayTurnState::PressedAButton);
    BindInputAction(ControllerButton::A, Action::RELEASED, &GetFSM(), this, &PlayerPlayTurnState::ReleasedAButton);
}

void PlayerPlayTurnState::OnStart() 
{
    
}

void PlayerPlayTurnState::OnEnter() 
{
    _inputState = InputState::ACTION_SELECTION;
    _setImguiPosCenter = true;
    _attackRemaining   = 0;

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
    default:
    case PlayerPlayTurnState::InputState::NONE:
        break;
    case PlayerPlayTurnState::InputState::ACTION_SELECTION:
        UpdateAttackButtonHeld(dt);
        break;
    case PlayerPlayTurnState::InputState::QUICK_TIME_EVENT:
        UpdateQuickTimeEventUI(dt);
        break;
    }

    Vector3 delta = Vector3(0, 1080, 0) * Mathf::Deg2Rad * dt;
    GetFSM().gameObject->transform->Rotation *= Quaternion::CreateFromYawPitchRoll(delta);
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
            }
        }
    }
}

void PlayerPlayTurnState::PressedAButton(const Input::Controller& controller)
{
    if (_inputState == InputState::ACTION_SELECTION)
    {
        _isDownAttackButton = true;
    }

}

void PlayerPlayTurnState::ReleasedAButton(const Input::Controller& controller) 
{
    if (_inputState == InputState::ACTION_SELECTION)
    {
        _attackButtonHeldTime = 0.f;
        _isDownAttackButton = false;
    }
}

void PlayerPlayTurnState::UpdateActionSelectionUI(float dt) 
{
    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49");
    {
        ImGui::Text((const char*)u8"A를 눌러 공격 진입");
        float t = _attackButtonHeldWaitTime / _attackButtonHeldTime;
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

            ImGui::Text((const char*)u8"남은 공격 횟수 : %d", _attackRemaining);

        }
        else
        {
            UmLogger.Message(LogLevel::LEVEL_DEBUG, u8"Weapon System이 존재하지 않습니다.");
        }
    }
    ImGui::End();
}
