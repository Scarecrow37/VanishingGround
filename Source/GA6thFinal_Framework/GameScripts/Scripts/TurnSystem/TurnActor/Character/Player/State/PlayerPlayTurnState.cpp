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
    auto& player = GetPlayer();
}

void PlayerPlayTurnState::OnEnter() 
{
    auto& player = GetPlayer();
    player.GetTokenSystem().OnTurnStart(&player);
}

void PlayerPlayTurnState::OnExit() 
{
    _isStart = false;

    auto& player = GetPlayer();
    player.GetTokenSystem().OnTurnEnd(&player);
}

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
            auto enemies = GameObject::FindGameObjectsWithTag(Enemy::TAG);
            for (auto& weak : enemies)
            {
                if (false == weak.expired())
                {
                    auto enemy = weak.lock();
                    if (enemy->IsValid())
                    {
                        auto enemyComponent = enemy->GetComponent<Enemy>();
                        if (nullptr != enemyComponent)
                        {
                            TurnActor::STATE state = enemyComponent->GetActorState();
                            if (state != TurnActor::STATE::Dead)
                            {
                                if (ImGui::Selectable(enemy->ToString().data()))
                                {
                                    selectTarget = enemyComponent;
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

                GetPlayer().EndTurn();
                selectTarget->Dead();
                selectTarget = nullptr;
            }
            if (ImGui::Button("Kill"))
            {
                GetPlayer().Dead();
            }
        }
        else
        {
            UmLogger.Message(LogLevel::LEVEL_DEBUG, u8"Weapon System이 존재하지 않습니다.");
        }
    }
    ImGui::End();

    Vector3 delta = Vector3(0, 1080, 0) * Mathf::Deg2Rad * UmTime.DeltaTime();
    GetFSM().gameObject->transform->Rotation *= Quaternion::CreateFromYawPitchRoll(delta);
}
