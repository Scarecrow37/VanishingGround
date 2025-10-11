#pragma once
#include "Utility/SingletonHelper.h"

class TextElement;
class DescriptionPanel;
class WeaponElement;
class ImageElement;
class PlayerWeaponChangeNavi;
class WeaponChangeUIManager : public Component, public InputReceiver
{
    USING_PROPERTY(WeaponChangeUIManager)
    SingletonComponent<WeaponChangeUIManager> _singletonComponent{this};

public:
    struct PlayerWeaponInfo
    {
        ImageElement* FocusImage = nullptr;
        ImageElement* IconImage  = nullptr;
        PlayerWeaponChangeNavi* WeaponNavi = nullptr;
    };

    struct PlayerWeaponStats
    {
        TextElement*      Name        = nullptr;
        ImageElement*     Icon        = nullptr;
        TextElement*      Damage      = nullptr;
        TextElement*      Critical    = nullptr;
        TextElement*      AttackCount = nullptr;
        TextElement*      Speed       = nullptr;
        DescriptionPanel* Description = nullptr;
    };

public:
    inline static constexpr const char* TAG = "Weapon Change UI Manager";

    WeaponChangeUIManager();
    ~WeaponChangeUIManager() override;

public:
    void ShowWeaponChangeUI(const std::string& changeWeaponName);
    void ShowWeaponChangeUI(const WeaponElement& changeWeapon);

    void SetPlayerWeaponStatsUI(const WeaponElement& focusWeapon);

    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(WeaponChangeUIManager)

    void ImGuiDrawPropertysEvent() override;
    void Reset() override;
    void Awake() override;
    void Start() override;
  
private:
    void FindUIElements();

private:
    std::vector<PlayerWeaponInfo> _playerWeapons;
    PlayerWeaponStats             _playerWeaponStats;

};

