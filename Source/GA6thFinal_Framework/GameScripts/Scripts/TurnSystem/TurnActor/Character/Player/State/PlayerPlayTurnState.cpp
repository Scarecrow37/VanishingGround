#include "pchScripts.h"
#include "PlayerPlayTurnState.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

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
}

void PlayerPlayTurnState::OnEnter() 
{
    
}

void PlayerPlayTurnState::OnExit() 
{
    _isStart = false;
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
        static std::string selectTarget{STR_NULL};
        if (ImGui::BeginCombo("Target Enemy##9A48EE30-CB5F-48AC-9740-DDF8118AAC49", selectTarget.data()))
        {
            auto enemys = GameObject::FindGameObjectsWithTag(Enemy::TAG);
            for (auto& weak : enemys)
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
                                    selectTarget = enemy->ToString();
                                }
                            }
                        }
                    }
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Attack") && selectTarget != STR_NULL)
        {
            UmLogger.Message(LogLevel::LEVEL_DEBUG,
                             std::format("{}{}{}", u8"플레이어가 "_c_str, selectTarget.data(), u8"을 공격!"_c_str));
            GetPlayer().EndTurnPlayer();
        }
    }
    ImGui::End();

    Vector3 delta = Vector3(0, 1080, 0) * Mathf::Deg2Rad * UmTime.DeltaTime();
    GetFSM().gameObject->transform->Rotation *= Quaternion::CreateFromYawPitchRoll(delta);
}
