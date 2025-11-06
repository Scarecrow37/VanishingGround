#pragma once
#include "UI/Contents/TooltipComponent.h"
#include "Utility/SingletonHelper.h"

class TooltipGroupComponent;

class TooltipSystem : public Component
{
    USING_PROPERTY(TooltipSystem)

    static constexpr std::u8string_view DATA_TOOLTIP_SHEET_NAME                = u8"툴팁";
    static constexpr std::u8string_view DATA_TOOLTIP_COLUMN_KEY_ID             = u8"ID";
    static constexpr std::u8string_view DATA_TOOLTIP_COLUMN_KEY_ICON_ID        = u8"IconID";
    static constexpr std::u8string_view DATA_TOOLTIP_COLUMN_KEY_NAME_ID        = u8"NameID";
    static constexpr std::u8string_view DATA_TOOLTIP_COLUMN_KEY_DESCRIPTION_ID = u8"DescriptionID";
    static constexpr std::u8string_view DATA_TEXT_SHEET_NAME                   = u8"텍스트";
    static constexpr std::u8string_view DATA_TEXT_COLUMN_KEY_ID                = u8"ID";
    static constexpr std::u8string_view DATA_TEXT_COLUMN_CONTENT               = u8"Content";
    static constexpr std::u8string_view DATA_TEXT_COLUMN_COLOR                 = u8"Color";

public:
    TooltipSystem();

public:
    REFLECT_PROPERTY()

public:
    void RegisterTooltipGroup(Tooltip::Group group, const std::weak_ptr<TooltipGroupComponent>& component);

    void Show(Tooltip::Group group, int id) const;
    void Show(Tooltip::Group group, std::initializer_list<int> ids) const;
    void Show(Tooltip::Group group, std::span<const int> ids) const;

    void Hide();
    void Hide(Tooltip::Group group);

    Tooltip::TooltipData GetTooltip(int id) const;

protected:
    void Awake() override;
    void Start() override;
    void ImGuiDrawPropertysEvent() override;

private:
    void SetupData();

    void ShowDataProperty();
    void ShowTestTooltipProperty();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TooltipSystem)

private:
    SingletonObject<TooltipSystem> _singletonObject{this};
    SingletonComponent<TooltipSystem> _singletonComponent{this};

    std::unordered_map<Tooltip::Group, std::weak_ptr<TooltipGroupComponent>> _tooltipGroups;
    std::unordered_map<int, Tooltip::TooltipData>                   _tooltips;
};