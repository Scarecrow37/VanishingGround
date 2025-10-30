#pragma once
#include <CombatUIManager/UIGroup.h>
class OverlayPanel;
class ImageElement;
class FadeUIComponent;
class SpawnDamagePanel;

namespace CombatUI
{
    struct CharacterHUDGroup : public UIGroup
    {
        GameObject*     Root = nullptr;

        OverlayPanel*   PlayerHUDPanel = nullptr;
        Vector3         PlayerPosition = Vector3::Zero;

        OverlayPanel*   EnemyHUDPanel[3] = {nullptr, nullptr, nullptr}; // Left, Middle, Right
        Vector3         EnemyPosition[3] = {Vector3::Zero, Vector3::Zero, Vector3::Zero};

        FadeUIComponent* FadeUI = nullptr;

        SpawnDamagePanel* PlayerSpawnDamagePanel = nullptr;
        SpawnDamagePanel* EnemySpawnDamagePanel[3] = {nullptr, nullptr, nullptr};

        SpawnDamagePanel* PlayerSpawnHealPanel = nullptr;
        SpawnDamagePanel* EnemySpawnHealPanel[3] = {nullptr, nullptr, nullptr};

        SpawnDamagePanel* EnemySpawnCriticalDamage[3] = {nullptr, nullptr, nullptr};

        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;

        void RefreshUIPosition();
        bool RefreshEnemiesPosition();

        void FadeIn(float duration) override;
        void FadeOut(float duration) override;
    };
} // namespace CombatUI