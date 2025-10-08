#pragma once
#include <CombatUIManager/UIGroup.h>
class OverlayPanel;
class ImageElement;

namespace CombatUI
{
    struct CharacterHUDGroup : public UIGroup
    {
        OverlayPanel* GroupPanel = nullptr;

        OverlayPanel* PlayerHUDPanel = nullptr;
        Vector3       PlayerPosition = Vector3::Zero;

        OverlayPanel* EnemyHUDPanel[3] = {nullptr, nullptr, nullptr}; // Left, Middle, Right
        Vector3       EnemyPosition[3] = {Vector3::Zero, Vector3::Zero, Vector3::Zero};

        bool FindUI() override;
        bool IsValid() override;
        void ActiveUI(bool active) override;

        void RefreshUIPosition();
        bool RefreshEnemiesPosition();
    };
} // namespace CombatUI