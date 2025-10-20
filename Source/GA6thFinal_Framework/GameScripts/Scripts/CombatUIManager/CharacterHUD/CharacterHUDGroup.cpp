#include "pchScripts.h"
#include "CharacterHUDGroup.h"

#include <UI/Panels/Overlay/OverlayPanel.h>
#include <Camera/CameraComponent.h>

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

namespace CombatUI
{
    bool CharacterHUDGroup::FindUI()
    {
        Root = GameObject::FindWithTag("Character HUD Group").lock().get();
        if (Root)
        {
            // 그룹 패널 내부를 탐색
            Transform& transform = Root->transform;
            Transform::ForeachBFS(transform, [this](Transform* curr) {
                if (curr)
                {
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
                }
            });
        }

        return IsValid();
    }

    bool CharacterHUDGroup::IsValid() const 
    {
        return Root && PlayerHUDPanel && EnemyHUDPanel[0] && EnemyHUDPanel[1] && EnemyHUDPanel[2];
    }

    void CharacterHUDGroup::ActiveUI(bool active)
    {
        if (Root)
        {
            Root->ActiveSelf = active;
        }
    }

    void CharacterHUDGroup::RefreshUIPosition()
    {
        if (false == IsValid())
        {
            return;
        }

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
        if (CameraComponent* camera = CameraComponent::MainCamera())
        {
            if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
            {
                auto enemies = turnMode->GetEnemies();

                for (size_t i = 0; i < enemies.size(); ++i)
                {
                    if (enemies[i])
                    {
                        Monster::SpawnPoint spawnPoint = enemies[i]->SpawnPoint;
                        int                 index      = static_cast<int>(spawnPoint);
                        if (EnemyHUDPanel[index])
                        {
                            EnemyPosition[i] = camera->WorldToViewport(enemies[i]->transform->GetWorldPosition());
                        }

                    }
                }

                bool valid = PlayerHUDPanel;
                if (valid)
                {

                    if (Player* player = turnMode->GetPlayer())
                    {
                        const auto& playerWorld = player->transform->GetWorldPosition();
                        PlayerPosition          = camera->WorldToViewport(playerWorld);
                    }
                }
                return true;
            }
        }
        return false;
    }
} // namespace CombatUI