#pragma once
#include "Utility/SingletonHelper.h"
#include "WeaponSystem/WeaponElement/WeaponElement.h"

class TextElement;
class DescriptionPanel;
class ImageElement;
class PlayerWeaponChangeNavi;
class ItemInfoUIManager;
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
        ItemInfoUIManager* Manager = nullptr;
    };

    struct ChangeWeaponStats
    {
        struct StatsUIDatas
        {
            TextElement*  Text      = nullptr;
            ImageElement* UpArrow   = nullptr;
            ImageElement* DownArrow = nullptr;
        };
        ItemInfoUIManager* Manager = nullptr;
        StatsUIDatas       Damage;
        StatsUIDatas       Critical;
        StatsUIDatas       AttackCount;
        StatsUIDatas       Speed;
    };

    struct WarningUI
    {
        GameObject*   WarningUIObject = nullptr;
        ImageElement* Before          = nullptr;
        ImageElement* After           = nullptr;
    };

public:
    inline static constexpr const char* TAG = "Weapon Change UI Manager";

    WeaponChangeUIManager();
    ~WeaponChangeUIManager() override;

public:
    void ShowWeaponChangeUI(const std::string& changeWeaponName);
    void ShowWeaponChangeUI(const WeaponElement& changeWeapon);

    void ShowChangeWarningUI(int slot); 
    bool HasWarningUI() const;

    void SetPlayerWeaponStatsUI(const WeaponElement& focusWeapon);


    REFLECT_PROPERTY()

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(WeaponChangeUIManager)

    void ImGuiDrawPropertysEvent() override;
    void Reset() override;
    void Awake() override;
    void Start() override;
    void Update() override;
  
private:
    void HideUI();

    void FindUIElements();
    void OnPressedActionB(const Input::Controller&);
    void OnPressedActionA(const Input::Controller&);

private:
    ImageElement*                 _changeWeaponIcon;
    std::vector<PlayerWeaponInfo> _playerWeapons;
    PlayerWeaponStats             _playerWeaponStats;
    ChangeWeaponStats             _changeWeaponStats;
    WeaponElement                 _changeWeaponElement;
    int                           _changeWeaponSlot;
    WarningUI                     _warningUI;

    enum class UIState
    {
        IDLE,
        CANCEL,
        APPROVE
    }
    _state;
};

