#include "pchScripts.h"
#include "CharacterHUDGroup.h"

#include <UI/Panels/Overlay/OverlayPanel.h>
#include <Camera/CameraComponent.h>

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <UI/Animations/FadeUIComponent/FadeUIComponent.h>
#include <UI/Contents/SpawnDamagePanel.h>

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
                    else if (curr->gameObject->CompareTag("Player Spawn Damage UI"))
                    {
                        PlayerSpawnDamagePanel = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Left Spawn Damage UI"))
                    {
                        EnemySpawnDamagePanel[0] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Middle Spawn Damage UI"))
                    {
                        EnemySpawnDamagePanel[1] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Right Spawn Damage UI"))
                    {
                        EnemySpawnDamagePanel[2] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if(curr->gameObject->CompareTag("Player Spawn Heal UI"))
                    {
                        PlayerSpawnHealPanel = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Left Spawn Heal UI"))
                    {
                        EnemySpawnHealPanel[0] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Middle Spawn Heal UI"))
                    {
                        EnemySpawnHealPanel[1] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Right Spawn Heal UI"))
                    {
                        EnemySpawnHealPanel[2] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Left Spawn Critical Damage UI"))
                    {
                        EnemySpawnCriticalDamage[0] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Middle Spawn Critical Damage UI"))
                    {
                        EnemySpawnCriticalDamage[1] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Right Spawn Critical Damage UI"))
                    {
                        EnemySpawnCriticalDamage[2] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                }
            });

            FadeUI = Root->GetComponent<FadeUIComponent>();
        }

        return IsValid();
    }

    bool CharacterHUDGroup::IsValid() const 
    {
        return Root && PlayerHUDPanel && EnemyHUDPanel[0] && EnemyHUDPanel[1] && EnemyHUDPanel[2] &&
               PlayerSpawnDamagePanel && EnemySpawnDamagePanel[0] && EnemySpawnDamagePanel[1] &&
               EnemySpawnDamagePanel[2] && PlayerSpawnHealPanel && EnemySpawnHealPanel[0] && EnemySpawnHealPanel[1] &&
               EnemySpawnHealPanel[2] && EnemySpawnCriticalDamage[0] && EnemySpawnCriticalDamage[1] &&
               EnemySpawnCriticalDamage[2] && FadeUI;
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
                const auto& enemies = turnMode->GetEnemies();

                for (size_t i = 0; i < enemies.size(); ++i)
                {
                    if (enemies[i])
                    {
                        Monster::SpawnPoint spawnPoint = enemies[i]->SpawnPoint;
                        int                 index      = static_cast<int>(spawnPoint);
                        if (EnemyHUDPanel[index])
                        {
                            EnemyPosition[index] = camera->WorldToViewport(enemies[i]->transform->GetWorldPosition());
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

    void CharacterHUDGroup::FadeIn(float duration) 
    {
        if (FadeUI)
        {
            FadeUI->FadeDuration = duration;
            FadeUI->FadeIn();
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Fade UI Component가 존재하지 않습니다.");
        }
    }

    void CharacterHUDGroup::FadeOut(float duration) 
    {
        if (FadeUI)
        {
            FadeUI->FadeDuration = duration;
            FadeUI->FadeOut();
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Fade UI Component가 존재하지 않습니다.");
        }
    }
} // namespace CombatUI