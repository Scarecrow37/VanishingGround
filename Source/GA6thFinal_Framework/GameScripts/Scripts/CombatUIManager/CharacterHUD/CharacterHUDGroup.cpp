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
            Overlay = Root->GetComponent<OverlayPanel>();
            FadeUI  = Root->GetComponent<FadeUIComponent>();
            // 그룹 패널 내부를 탐색
            Transform& transform = Root->transform;
            Transform::ForeachBFS(transform, [this](Transform* curr) {
                if (curr)
                {
                    for (size_t i = 0; i < 3; ++i)
                    {
                        // HUD
                        if (curr->gameObject->CompareTag(MONSTER_HUD[i]))
                        {
                            EnemyHUDPanel[i] = curr->gameObject->GetComponent<OverlayPanel>();
                        }
                        // 데미지 텍스트 스포너
                        else if (curr->gameObject->CompareTag(MONSTER_ACTION_HUD[i]))
                        {
                            EnemyActionPanel[i] = curr->gameObject->GetComponent<OverlayPanel>();
                        }
                        // 데미지 텍스트 스포너
                        else if (curr->gameObject->CompareTag(MONSTER_SPAWN_DAMAGE_HUD[i]))
                        {
                            EnemySpawnDamagePanel[i] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                        }
                        // 치명타 데미지 텍스트 스포너
                        else if (curr->gameObject->CompareTag(MONSTER_SPAWN_CRIT_DAMAGE_HUD[i]))
                        {
                            EnemySpawnCriticalDamage[i] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                        }
                        // 힐 데미지 텍스트 스포너
                        else if (curr->gameObject->CompareTag(MONSTER_SPAWN_HEAL_HUD[i]))
                        {
                            EnemySpawnHealPanel[i] = curr->gameObject->GetComponent<SpawnDamagePanel>();
                        }
                    }
                    if (curr->gameObject->CompareTag("Player HUD"))
                    {
                        PlayerHUDPanel = curr->gameObject->GetComponent<OverlayPanel>();
                    }
                    else if (curr->gameObject->CompareTag("Player Spawn Damage UI"))
                    {
                        PlayerSpawnDamagePanel = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Player Spawn Heal UI"))
                    {
                        PlayerSpawnHealPanel = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                }
            });
        }

        return IsValid();
    }

    bool CharacterHUDGroup::IsValid() const 
    {
        return Root && PlayerHUDPanel && EnemyHUDPanel[0] && EnemyHUDPanel[1] && EnemyHUDPanel[2] &&
               PlayerSpawnDamagePanel && EnemySpawnDamagePanel[0] && EnemySpawnDamagePanel[1] &&
               EnemySpawnDamagePanel[2] && PlayerSpawnHealPanel && EnemySpawnHealPanel[0] && EnemySpawnHealPanel[1] &&
               EnemySpawnHealPanel[2] && EnemySpawnCriticalDamage[0] && EnemySpawnCriticalDamage[1] &&
               EnemySpawnCriticalDamage[2] && FadeUI && EnemyActionPanel[0] && EnemyActionPanel[1] &&
               EnemyActionPanel[2];
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
        SIZE    hudPanelSize[3];
        POINT   hudPanelPoint[3];
        SIZE    actionPanelSize[3];
        POINT   actionPanelPoint[3];

        const int left      = 0;
        const int middle    = 1;
        const int right     = 2;

        if (EnemyHUDPanel[1])
        {
            {
                //////////////////////
                //  Middle Panel    //
                //////////////////////
                if (EnemyHUDPanel[middle])
                {
                    hudPanelSize[middle]         = EnemyHUDPanel[middle]->Size;
                    hudPanelPoint[middle]        = {(LONG)EnemyFootPosition[1].x - hudPanelSize[middle].cx / 2,
                                                   (LONG)EnemyFootPosition[1].y};
                    EnemyHUDPanel[middle]->Point = hudPanelPoint[middle];
                }
                if (EnemyActionPanel[middle])
                {
                    actionPanelSize[middle]      = EnemyActionPanel[middle]->Size;
                    actionPanelPoint[middle]     = {(LONG)EnemyHeadPosition[middle].x - actionPanelSize[middle].cx / 2,
                                                   (LONG)EnemyHeadPosition[middle].y};
                    EnemyActionPanel[middle]->Point = actionPanelPoint[middle];
                }
            }
            {
                //////////////////////
                //  Left Panel      //
                //////////////////////
                if (EnemyHUDPanel[left])
                {
                    hudPanelSize[left]            = EnemyHUDPanel[left]->Size;
                    hudPanelPoint[left]           = {hudPanelPoint[middle].x - hudPanelSize[left].cx - MONSTER_HUD_SPACE_X,
                                                     hudPanelPoint[middle].y + MONSTER_HUD_SPACE_Y};
                    EnemyHUDPanel[left]->Point    = hudPanelPoint[left];
                }
                if (EnemyActionPanel[left])
                {
                    actionPanelSize[left]         = EnemyActionPanel[left]->Size;
                    actionPanelPoint[left]        = {actionPanelPoint[middle].x - actionPanelSize[left].cx - MONSTER_HUD_SPACE_X,
                                                    (LONG)EnemyHeadPosition[left].y};
                    EnemyActionPanel[left]->Point = actionPanelPoint[left];
                }
            }
            {
                //////////////////////
                //  Right Panel     //
                //////////////////////
                if (EnemyHUDPanel[right])
                {
                    hudPanelSize[right]         = EnemyHUDPanel[right]->Size;
                    hudPanelPoint[right]        = {hudPanelPoint[middle].x + hudPanelSize[middle].cx + MONSTER_HUD_SPACE_X,
                                                  hudPanelPoint[middle].y + MONSTER_HUD_SPACE_Y};
                    EnemyHUDPanel[right]->Point = hudPanelPoint[right];
                }
                if (EnemyActionPanel[right])
                {
                    actionPanelSize[right]         = EnemyActionPanel[right]->Size;
                    actionPanelPoint[right]        = {actionPanelPoint[middle].x + actionPanelSize[middle].cx + MONSTER_HUD_SPACE_X,
                                                     (LONG)EnemyHeadPosition[right].y};
                    EnemyActionPanel[right]->Point = actionPanelPoint[right];
                }
            }
        }
    }

    bool CharacterHUDGroup::RefreshCharactersUIPosition()
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
                        const int     index      = static_cast<int>((Monster::SpawnPoint)enemies[i]->SpawnPoint);
                        const Vector3 foot       = enemies[i]->transform->GetWorldPosition();
                        const Vector3 head       = foot + GetHeadOffset(enemies[i]);
                        EnemyFootPosition[index] = camera->WorldToViewport(foot);
                        EnemyHeadPosition[index] = camera->WorldToViewport(head);
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
            FadeUI->ReFindTargets();
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
            FadeUI->ReFindTargets();
            FadeUI->FadeOut();
        }
        else
        {
            UmLogger.Log(LogLevel::LEVEL_WARNING, u8"Fade UI Component가 존재하지 않습니다.");
        }
    }
    Vector3 CharacterHUDGroup::GetHeadOffset(Enemy* enemy)
    {
        if (enemy)
        {
            const EnemyType type = enemy->Type;
            const Vector3   scale = enemy->transform->Scale;
            const Vector3   offset = Vector3(0.0f, 1.0f, 0.0f);
            switch (type)
            {
            case EnemyType::MONSTER_A:
                return Vector3(0.0f, 2.0f, 0.0f) * scale + offset;
            case EnemyType::MONSTER_B:
                return Vector3(0.0f, 2.0f, 0.0f) * scale + offset;
            case EnemyType::MONSTER_C:
                return Vector3(0.0f, 3.0f, 0.0f) * scale + offset;
            default:
                break;
            }
        }
        return Vector3();
    }
} // namespace CombatUI