#include "pchScripts.h"
#include "CharactorHUDGroup.h"

#include <UI/Panels/Overlay/OverlayPanel.h>
#include <Camera/CameraComponent.h>

#include <BattleSystem/Battle.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

bool CharacterHUDGroup::FindUI() 
{
    auto hudGroup = GameObject::FindWithTag("Character HUD Group").lock();
    if (hudGroup)
    {
        GroupPanel = hudGroup->GetComponent<OverlayPanel>();

        // 그룹 패널 내부를 탐색
        Transform& transform = hudGroup->transform;
        Transform::ForeachBFS(transform, [this](Transform* curr) {
            if (curr->gameObject->CompareTag("Left Monster HUD"))
            {
                EnemyHUDPanel[0] = curr->gameObject->GetComponent<OverlayPanel>();
            }
            else if (curr->gameObject->CompareTag("Middle Monster HUD"))
            {
                EnemyHUDPanel[1] = curr->gameObject->GetComponent<OverlayPanel>();
            }
            else if (curr->gameObject->CompareTag("Right Monster HUD"))
            {
                EnemyHUDPanel[2] = curr->gameObject->GetComponent<OverlayPanel>();
            }
            else if (curr->gameObject->CompareTag("Player HUD"))
            {
                PlayerHUDPanel = curr->gameObject->GetComponent<OverlayPanel>();
            }
        });
    }

    return IsValid();
}

bool CharacterHUDGroup::IsValid()
{
    return GroupPanel && PlayerHUDPanel && EnemyHUDPanel[0] && EnemyHUDPanel[1] && EnemyHUDPanel[2];
}

void CharacterHUDGroup::ActiveUI(bool active) 
{
    if (IsValid())
    {
        // TODO: 그룹 패널 활성화/비활성화
    }
}

void CharacterHUDGroup::RefreshUIPosition()
{
    POINT       point;
    const SIZE& resolution = UmGraphics.GetResolution();

    for (size_t i = 0; i < 3; ++i)
    {
        const float offsetY = -150.0f;
        if (EnemyHUDPanel[i])
        {
            SIZE size               = EnemyHUDPanel[i]->Size;
            point.x                 = (LONG)(EnemyPosition[i].x - size.cx / 2);
            point.y                 = (LONG)(EnemyPosition[i].y - size.cy + offsetY);
            EnemyHUDPanel[i]->Point = point;
        }
    }

    if (PlayerHUDPanel)
    {
        const float offsetY   = -800.0f;
        SIZE        size      = PlayerHUDPanel->Size;
        point.x               = (LONG)(PlayerPosition.x - size.cx / 2);
        point.y               = (LONG)(PlayerPosition.y - size.cy + offsetY);
        PlayerHUDPanel->Point = point;
    }
}

bool CharacterHUDGroup::RefreshEnemiesPosition()
{
    CameraComponent* camera = CameraComponent::MainCamera();
    if (camera)
    {
        auto enemies = Battle::GetTargetsFromFlags(Battle::ENEMY_TARGET_FLAG_ALL);

        for (size_t i = 0; i < enemies.size(); ++i)
        {
            bool valid = enemies[i] && EnemyHUDPanel[i];
            if (valid)
            {
                EnemyPosition[i] = camera->WorldToViewport(enemies[i]->transform->GetWorldPosition());
            }
        }

        bool valid = PlayerHUDPanel;
        if (valid)
        {
            if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
            {
                if (Player* player = turnMode->GetPlayer())
                {
                    const auto& playerWorld = player->transform->GetWorldPosition();
                    PlayerPosition          = camera->WorldToViewport(playerWorld);
                }
            }
        }
        return true;
    }
    return false;
}
