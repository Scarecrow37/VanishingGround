#pragma once
#include "UI/Contents/TooltipComponent.h"
#include "Utility/SingletonHelper.h"

class TooltipGroupComponent;

class TooltipSystem : public Component
{
    USING_PROPERTY(TooltipSystem)

    static constexpr std::string_view TOOLTIP_GROUP_WEAPON            = "Weapon Tooltip Group";
    static constexpr std::string_view TOOLTIP_GROUP_ACCESSORY         = "Accessory Tooltip Group";
    static constexpr std::string_view TOOLTIP_GROUP_PLAYER            = "Player Tooltip Group";
    static constexpr std::string_view TOOLTIP_GROUP_ENEMY_LEFT        = "Left Enemy Tooltip Group";
    static constexpr std::string_view TOOLTIP_GROUP_ENEMY_MIDDLE      = "Middle Enemy Tooltip Group";
    static constexpr std::string_view TOOLTIP_GROUP_ENEMY_RIGHT       = "Right Enemy Tooltip Group";
    static constexpr std::string_view TOOLTIP_GROUP_REVELATION_UP     = "Up Revelation Tooltip Group";
    static constexpr std::string_view TOOLTIP_GROUP_REVELATION_MIDDLE = "Middle Revelation Tooltip Group";
    static constexpr std::string_view TOOLTIP_GROUP_REVELATION_DOWN   = "Down Revelation Tooltip Group";

    static constexpr std::u8string_view DATA_TOOLTIP_SHEET_NAME                = u8"툴팁";
    static constexpr std::u8string_view DATA_TOOLTIP_COLUMN_KEY_ID             = u8"ID";
    static constexpr std::u8string_view DATA_TOOLTIP_COLUMN_KEY_ICON_ID        = u8"IconID";
    static constexpr std::u8string_view DATA_TOOLTIP_COLUMN_KEY_NAME_ID        = u8"NameID";
    static constexpr std::u8string_view DATA_TOOLTIP_COLUMN_KEY_DESCRIPTION_ID = u8"DescriptionID";
    static constexpr std::u8string_view DATA_TEXT_SHEET_NAME                   = u8"텍스트";
    static constexpr std::u8string_view DATA_TEXT_COLUMN_KEY_ID                = u8"ID";
    static constexpr std::u8string_view DATA_TEXT_COLUMN_CONTENT               = u8"Content";

public:
    enum class Group
    {
        WEAPON,
        ACCESSORY,
        PLAYER,
        ENEMY_LEFT,
        ENEMY_MIDDLE,
        ENEMY_RIGHT,
        REVELATION_UP,
        REVELATION_MIDDLE,
        REVELATION_DOWN
    };

public:
    TooltipSystem();

public:
    REFLECT_PROPERTY()

public:
    void Show(Group group, int id);
    void Show(Group group, std::initializer_list<int> ids);
    void Show(Group group, std::span<const int> ids);

    void Hide();
    void Hide(Group group);

    TooltipComponent::TooltipData GetTooltip(int id) const;

protected:
    void Awake() override;
    void Start() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void FindComponents();
    void SetupData();

    void ShowDataProperty();
    void ShowTestTooltipProperty();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TooltipSystem)

private:
    SingletonObject<TooltipSystem> _singletonObject{this};
    SingletonComponent<TooltipSystem> _singletonComponent{this};

    std::unordered_map<Group, std::weak_ptr<TooltipGroupComponent>> _tooltipGroups;
    std::unordered_map<int, TooltipComponent::TooltipData>          _tooltips;

    std::unordered_map<Group, std::unordered_set<int>> _activeTooltips;
};