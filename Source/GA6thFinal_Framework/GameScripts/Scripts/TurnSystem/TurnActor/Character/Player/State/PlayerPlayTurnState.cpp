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

}

PlayerPlayTurnState::~PlayerPlayTurnState() 
{

}

void PlayerPlayTurnState::OnAwake() 
{

}

void PlayerPlayTurnState::OnStart() 
{
    _isStart = true;
    _isAttacking = false;
}

void PlayerPlayTurnState::OnEnter() 
{
}

void PlayerPlayTurnState::OnExit() 
{
    _isStart = false;
    _isAttacking = false;
}
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>

void PlayerPlayTurnState::OnUpdate() 
{
    if (_isStart)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 centerPos = ImVec2(viewport->Pos.x + (viewport->Size.x - 500.0f) * 0.5f,
                                  viewport->Pos.y + (viewport->Size.y - 500.0f) * 0.5f);

        ImGui::SetNextWindowPos(centerPos);
        ImGui::SetNextWindowSize(ImVec2(500, 500));

        _isStart = false;
    }

    ImGui::Begin("Player Turn##9A48EE30-CB5F-48AC-9740-DDF8118AAC49");
    {
        static Enemy* selectTarget{nullptr};
        std::string_view selectName = STR_NULL;
        if (selectTarget)
        {
            selectName = selectTarget->gameObject->Name;
        }

        if (ImGui::BeginCombo("Target Enemy##9A48EE30-CB5F-48AC-9740-DDF8118AAC49", selectName.data()))
        {
            auto* turnMode = TurnMode::GetInstance();
            if (turnMode)
            {
                auto* combatState = turnMode->States->CombatStartPhase;
                if (combatState)
                {
                    auto& enemies = combatState->GetEnemies();
                    for (auto& enemy : enemies)
                    {
                        if (enemy)
                        {
                            TurnActor::STATE state = enemy->GetActorState();
                            if (state != TurnActor::STATE::Dead)
                            {
                                std::string_view name = enemy->gameObject->ToString();
                                if (ImGui::Selectable(name.data()))
                                {
                                    selectTarget = enemy;
                                }
                                // 비어있으면 첫 번째 적을 select
                                if (nullptr == selectTarget)
                                {
                                    selectTarget = enemy;
                                }
                            }
                        }
                    }
                }
            }
            ImGui::EndCombo();
        }

        WeaponSystem* weaponSystem = WeaponSystem::GetInstance();
        if (weaponSystem)
        {
            Player&      player = GetPlayer();
            WeaponStats& weapon = const_cast<WeaponStats&>(weaponSystem->GetCurrentWeaponStats());
            weapon.ImGuiDrawPropertys();
            if (ImGui::Button("Attack") && selectTarget != nullptr)
            {
                UmLogger.Message(LogLevel::LEVEL_DEBUG,
                                 std::format("{}{}{}", u8"플레이어가 "_c_str, selectName.data(), u8"을 공격!"_c_str));

                TestAttack(selectTarget, 30);
                selectTarget = nullptr;
            }
            if (ImGui::Button("Kill"))
            {
                GetPlayer().TakeDamage(std::numeric_limits<int>::max());
            }
        }
        else
        {
            UmLogger.Message(LogLevel::LEVEL_DEBUG, u8"Weapon System이 존재하지 않습니다.");
        }
    }
    ImGui::End();

    if (_isAttacking)
    {
        Player& player = GetPlayer();
        if (true == player.IsAnimationEnd())
        {
            player.EndTurn();
        }
    }
    //Vector3 delta = Vector3(0, 1080, 0) * Mathf::Deg2Rad * UmTime.DeltaTime();
    //GetFSM().gameObject->transform->Rotation *= Quaternion::CreateFromYawPitchRoll(delta);
}

void PlayerPlayTurnState::TestAttack(Enemy* dest, int damage)
{
    Player& player = GetPlayer();
    player.SetAnimation(CharacterBase::ATTACK_1, false);
    _isAttacking = true;
    if (dest)
    {
        dest->TakeDamage(damage);
    }
}
