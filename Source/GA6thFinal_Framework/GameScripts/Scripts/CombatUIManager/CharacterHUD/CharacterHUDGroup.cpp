#include "pchScripts.h"
#include "CharacterHUDGroup.h"

#include <UI/Panels/Overlay/OverlayPanel.h>
#include <UI/Animations/FadeImageElement/FadeImageElement.h>
#include <Camera/UmCineMotion.h>

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Monster/System/MonsterSystem.h>

#include <UI/Animations/FadeUIComponent/FadeUIComponent.h>
#include <UI/Contents/SpawnDamagePanel.h>
#include <UI/Contents/SpawnTokenPanel.h>

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
                        // 포커스 HUD
                        else if (curr->gameObject->CompareTag(MONSTER_FOCUS_HUD[i]))
                        {
                            FocusEnemyHUDPanel[i] = curr->gameObject->GetComponent<OverlayPanel>();
                            FocusEnemyHUDFade[i]  = curr->gameObject->GetComponent<FadeUIComponent>();
                        }
                        else if (curr->gameObject->CompareTag(MONSTER_SPAWN_TOKEN_HUD[i]))
                        {
                            EnemySpawnTokenPanel[i] = curr->gameObject->GetComponent<SpawnTokenPanel>();
                        }
                    }
                    if (curr->gameObject->CompareTag("Player HUD"))
                    {
                        PlayerHUDPanel = curr->gameObject->GetComponent<OverlayPanel>();
                    }
                    else if (curr->gameObject->CompareTag("Player Focus HUD"))
                    {
                        FocusPlayerHUDPanel = curr->gameObject->GetComponent<OverlayPanel>();
                        FocusPlayerHUDFade  = curr->gameObject->GetComponent<FadeUIComponent>();
                    }
                    else if (curr->gameObject->CompareTag("Player Spawn Damage UI"))
                    {
                        PlayerSpawnDamagePanel = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Player Spawn Heal UI"))
                    {
                        PlayerSpawnHealPanel = curr->gameObject->GetComponent<SpawnDamagePanel>();
                    }
                    else if (curr->gameObject->CompareTag("Player Spawn Token UI"))
                    {
                        PlayerSpawnTokenPanel = curr->gameObject->GetComponent<SpawnTokenPanel>();
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
               EnemySpawnCriticalDamage[2] && PlayerSpawnTokenPanel && EnemySpawnTokenPanel[0] &&
               EnemySpawnTokenPanel[1] && EnemySpawnTokenPanel[2] && FadeUI && EnemyActionPanel[0] &&
               EnemyActionPanel[1] && EnemyActionPanel[2];
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
        SIZE    hudPanelSize[3] = {};
        POINT   hudPanelPoint[3] = {};
        SIZE    actionPanelSize[3] = {};
        POINT   actionPanelPoint[3] = {};

        const int left   = 0;
        const int middle = 1;
        const int right  = 2;

        //////////////////////
        //  Middle Panel    //
        //////////////////////
        if (EnemyHUDPanel[middle])
        {
            hudPanelSize[middle]         = EnemyHUDPanel[middle]->Size;
            hudPanelPoint[middle]        = {(LONG)EnemyFootPosition[middle].x - hudPanelSize[middle].cx / 2,
                                            (LONG)EnemyFootPosition[middle].y};
            EnemyHUDPanel[middle]->Point = hudPanelPoint[middle];
        }
        if (EnemyActionPanel[middle])
        {
            actionPanelSize[middle]       = EnemyActionPanel[middle]->Size;
            actionPanelPoint[middle]      = {(LONG)EnemyHeadPosition[middle].x - actionPanelSize[middle].cx / 2,
                                             (LONG)EnemyHeadPosition[middle].y};
            EnemyActionPanel[middle]->Point = actionPanelPoint[middle];
        }
        
        //////////////////////
        //  Left Panel      //
        //////////////////////
        if (EnemyHUDPanel[left])
        {
            hudPanelSize[left]          = EnemyHUDPanel[left]->Size;
            hudPanelPoint[left]         = {hudPanelPoint[middle].x - hudPanelSize[left].cx - MONSTER_HUD_SPACE_X,
                                           hudPanelPoint[middle].y + MONSTER_HUD_SPACE_Y};
            EnemyHUDPanel[left]->Point  = hudPanelPoint[left];
        }
        if (EnemyActionPanel[left])
        {
            actionPanelSize[left]       = EnemyActionPanel[left]->Size;
            actionPanelPoint[left]      = {actionPanelPoint[middle].x - actionPanelSize[left].cx - MONSTER_HUD_SPACE_X,
                                           (LONG)EnemyHeadPosition[left].y};
            EnemyActionPanel[left]->Point = actionPanelPoint[left];
        }

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
            actionPanelSize[right]      = EnemyActionPanel[right]->Size;
            actionPanelPoint[right]     = {actionPanelPoint[middle].x + actionPanelSize[middle].cx + MONSTER_HUD_SPACE_X,
                                           (LONG)EnemyHeadPosition[right].y};
            EnemyActionPanel[right]->Point = actionPanelPoint[right];
        }

        if (PlayerHUDPanel)
        {
            SIZE  size  = PlayerHUDPanel->Size;
            POINT point = {(LONG)(PlayerPosition.x - size.cx / 2),
                           (LONG)(PlayerPosition.y)};
            point.x     = std::max(point.x, (LONG)50);
            point.y     = std::max(point.y, (LONG)100);
            PlayerHUDPanel->Point = point;
        }
    }

    bool CharacterHUDGroup::RefreshCharactersUIPosition()
    {
        for (size_t i = 0; i < 3; ++i)
        {
            EnemyFootPosition[i] = Vector3::Zero;
            EnemyHeadPosition[i] = Vector3::Zero;
        }

        if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
        {
            // 연출 카메라 가져오기
            CameraComponent* camera = turnMode->GetBattleCamera();
            // 연출 카메라가 없다면 메인 카메라로
            if (nullptr == camera)
            {
                camera = CameraComponent::MainCamera();
            }
            if (camera)
            {
                if (MonsterSystem* system = SingletonComponent<MonsterSystem>::GetInstance())
                {
                    for (size_t i = 0; i < 3; ++i)
                    {
                        Monster::SpawnPoint spawnIndex = static_cast<Monster::SpawnPoint>(i);

                        auto weakEnemy      = system->GetSpawnedEnemyFromSpawnPoint(spawnIndex);
                        auto weakSpawnPoint = system->GetSpawnPointObject(spawnIndex);

                        Vector3 foot = Vector3::Zero;
                        Vector3 head = Vector3::Zero;

                        if (auto spawnPoint = weakSpawnPoint.lock().get())
                        {
                            foot = spawnPoint->transform->GetWorldPosition();
                        }
                        else
                        {
                            foot = Vector3(FLT_MIN, FLT_MIN, 0.0f);
                        }

                        if (auto enemy = weakEnemy.lock().get())
                        {
                            head = foot + GetHeadOffset(enemy);
                        }
                        else
                        {
                            head = foot;
                        }
                        EnemyFootPosition[i] = camera->WorldToViewport(foot);
                        EnemyHeadPosition[i] = camera->WorldToViewport(head);
                    }
                }
                if (Player* player = turnMode->GetPlayer())
                {
                    const Vector3 foot = player->transform->GetWorldPosition();
                    const Vector3 head = foot + GetHeadOffset(player);
                    PlayerPosition     = camera->WorldToViewport(head);
                }
            }
            return true;
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
            const Vector3   offset = Vector3(0.0f, 0.7f, 0.0f);
            switch (type)
            {
            case EnemyType::MONSTER_A:
                return Vector3(0.0f, 2.0f, 0.0f) * scale + offset;
            case EnemyType::MONSTER_B:
                return Vector3(0.0f, 1.5f, 0.0f) * scale + offset;
            case EnemyType::MONSTER_C:
                return Vector3(0.0f, 2.65f, 0.0f) * scale + offset;
            default:
                break;
            }
        }
        return Vector3();
    }
    Vector3 CharacterHUDGroup::GetHeadOffset(Player* player)
    {
        if (player)
        {
            const Vector3 scale = player->transform->Scale;
            const Vector3 offset = Vector3(0.0f, 0.3f, 0.0f);
            return Vector3(0.0f, 2.0f, 0.0f) * scale + offset;
        }
        return Vector3();
    }
    void CharacterHUDGroup::MonsterFocusIn(Monster::SpawnPoint spawnPoint, const float duration) 
    {
        int spawnIndex = static_cast<int>(spawnPoint);
        if (FocusEnemyHUDFade[spawnIndex])
        {
            if (FocusEnemyHUDPanel[spawnIndex] && EnemyHUDPanel[spawnIndex])
            {
                FocusEnemyHUDPanel[spawnIndex]->Point = EnemyHUDPanel[spawnIndex]->Point;
            }
            FocusEnemyHUDFade[spawnIndex]->FadeDuration = duration;
            FocusEnemyHUDFade[spawnIndex]->FadeIn();
        }
    }
    void CharacterHUDGroup::MonsterFocusOut(Monster::SpawnPoint spawnPoint, const float duration) 
    {
        int spawnIndex = static_cast<int>(spawnPoint);
        if (FocusEnemyHUDFade[spawnIndex])
        {
            FocusEnemyHUDFade[spawnIndex]->FadeDuration = duration;
            FocusEnemyHUDFade[spawnIndex]->FadeOut();
        }
    }
    void CharacterHUDGroup::PlayerFocusIn(const float duration) 
    {
        if (FocusPlayerHUDFade)
        {
            if (FocusPlayerHUDPanel && PlayerHUDPanel)
            {
                FocusPlayerHUDPanel->Point = PlayerHUDPanel->Point;
            }
            FocusPlayerHUDFade->FadeDuration = duration;
            FocusPlayerHUDFade->FadeIn();
        }
    }
    void CharacterHUDGroup::PlayerFocusOut(const float duration) 
    {
        if (FocusPlayerHUDFade)
        {
            FocusPlayerHUDFade->FadeDuration = duration;
            FocusPlayerHUDFade->FadeOut();
        }
    }
} // namespace CombatUI