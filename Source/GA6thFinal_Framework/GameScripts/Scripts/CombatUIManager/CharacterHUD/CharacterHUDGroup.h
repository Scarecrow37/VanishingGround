#pragma once
#include <CombatUIManager/UIGroup.h>
#include <Monster/Common/MonsterCommon.h>

class OverlayPanel;
class ImageElement;
class FadeUIComponent;
class SpawnDamagePanel;
class SpawnTokenPanel;
class Player;
class Enemy;

namespace CombatUI
{
    struct CharacterHUDGroup : public UIGroup
    {
        GameObject*         Root                            = nullptr;
        OverlayPanel*       Overlay                         = nullptr;

        FadeUIComponent*    FadeUI                          = nullptr;
        OverlayPanel*       PlayerHUDPanel                  = nullptr;

        OverlayPanel*       EnemyHUDPanel[3]                = {nullptr, nullptr, nullptr}; // Left, Middle, Right
        OverlayPanel*       EnemyActionPanel[3]             = {nullptr, nullptr, nullptr}; // Left, Middle, Right

        Vector3             PlayerPosition                  = Vector3::Zero;
        Vector3             EnemyFootPosition[3]            = {Vector3::Zero, Vector3::Zero, Vector3::Zero};
        Vector3             EnemyHeadPosition[3]            = {Vector3::Zero, Vector3::Zero, Vector3::Zero};

        SpawnDamagePanel*   PlayerSpawnHealPanel            = nullptr;
        SpawnDamagePanel*   PlayerSpawnDamagePanel          = nullptr;
        SpawnTokenPanel*    PlayerSpawnTokenPanel           = nullptr;

        SpawnDamagePanel*   EnemySpawnHealPanel[3]          = {nullptr, nullptr, nullptr};
        SpawnDamagePanel*   EnemySpawnDamagePanel[3]        = {nullptr, nullptr, nullptr};
        SpawnDamagePanel*   EnemySpawnCriticalDamage[3]     = {nullptr, nullptr, nullptr};
        SpawnTokenPanel*    EnemySpawnTokenPanel[3]         = {nullptr, nullptr, nullptr};

        OverlayPanel*       FocusEnemyHUDPanel[3]           = {nullptr, nullptr, nullptr};
        FadeUIComponent*    FocusEnemyHUDFade[3]            = {nullptr, nullptr, nullptr};
        OverlayPanel*       FocusPlayerHUDPanel             = nullptr;
        FadeUIComponent*    FocusPlayerHUDFade              = nullptr;

        bool FindUI() override;
        bool IsValid() const override;
        void ActiveUI(bool active) override;

        void RefreshUIPosition();
        bool RefreshCharactersUIPosition();

        void FadeIn(float duration) override;
        void FadeOut(float duration) override;

        Vector3 GetHeadOffset(Enemy* enemy);
        Vector3 GetHeadOffset(Player* player);

        void MonsterFocusIn(Monster::SpawnPoint spawnPoint, const float duration = 0.5f);
        void MonsterFocusOut(Monster::SpawnPoint spawnPoint, const float duration = 0.5f);
        void PlayerFocusIn(const float duration = 0.5f);
        void PlayerFocusOut(const float duration = 0.5f);

    private:
        inline static constexpr std::array<const char*, 3> MONSTER_HUD = {
            "Left Monster HUD", "Middle Monster HUD", "Right Monster HUD"
        };
        inline static constexpr std::array<const char*, 3> MONSTER_ACTION_HUD = {
            "Left Proclamation HUD", "Middle Proclamation HUD", "Right Proclamation HUD"
        };
        inline static constexpr std::array<const char*, 3> MONSTER_SPAWN_DAMAGE_HUD = {
            "Left Spawn Damage UI", "Middle Spawn Damage UI", "Right Spawn Damage UI"
        };
        inline static constexpr std::array<const char*, 3> MONSTER_SPAWN_CRIT_DAMAGE_HUD = {
            "Left Spawn Critical Damage UI", "Middle Spawn Critical Damage UI", "Right Spawn Critical Damage UI"
        };
        inline static constexpr std::array<const char*, 3> MONSTER_SPAWN_HEAL_HUD = {
            "Left Spawn Heal UI", "Middle Spawn Heal UI", "Right Spawn Heal UI"
        };
        inline static constexpr std::array<const char*, 3> MONSTER_SPAWN_TOKEN_HUD = {
            "Left Spawn Token UI", "Middle Spawn Token UI", "Right Spawn Token UI"
        };
        inline static constexpr std::array<const char*, 3> MONSTER_FOCUS_HUD = {
            "Left Focus HUD", "Middle Focus HUD", "Right Focus HUD"
        };

        inline static LONG MONSTER_HUD_SPACE_X = 0;
        inline static LONG MONSTER_HUD_SPACE_Y = -30;
    };
} // namespace CombatUI