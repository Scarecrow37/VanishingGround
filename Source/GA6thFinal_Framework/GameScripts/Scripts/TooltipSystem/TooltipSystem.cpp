#include "pchScripts.h"
#include "TooltipSystem.h"
#include "Scripts/UI/Contents/TooltipGroupComponent.h"
#include "ExcelDataSystem/ExcelDataSystem.h"

UMREAL_COMPONENT(TooltipSystem)

TooltipSystem::TooltipSystem() = default;

void TooltipSystem::Show(const Group group, const int id) const
{
    try
    {
        if (const auto sharedGroup = _tooltipGroups.at(group).lock())
        {
            sharedGroup->Show(_tooltips.at(id));
            sharedGroup->FadeIn();
        }
    }
    catch (const std::out_of_range& exception)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Tooltip group or tooltip data not found.");
        UmLogger.Log(LogLevel::LEVEL_WARNING, exception.what());
    }
}

void TooltipSystem::Show(const Group group, const std::initializer_list<int> ids) const
{
    try
    {
        if (const auto sharedGroup = _tooltipGroups.at(group).lock())
        {
            for (const int id : ids)
            {
                sharedGroup->Show(_tooltips.at(id));
            }
            sharedGroup->FadeIn();
        }
    }
    catch (const std::out_of_range& exception)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Tooltip group or tooltip data not found.");
        UmLogger.Log(LogLevel::LEVEL_WARNING, exception.what());
    }
}

void TooltipSystem::Hide()
{
    for (auto& weakGroup : _tooltipGroups | std::views::values)
    {
        if (const auto sharedGroup = weakGroup.lock())
        {
            sharedGroup->FadeOut();
            UmTime.Invoke(this, sharedGroup->GetFadeDuration(), [sharedGroup]() { sharedGroup->Hide(); });
        }
    }
}

void TooltipSystem::Hide(const Group group)
{
    try
    {
        if (const auto sharedGroup = _tooltipGroups.at(group).lock())
        {
            sharedGroup->FadeOut();
            UmTime.Invoke(this, sharedGroup->GetFadeDuration(), [sharedGroup]() { sharedGroup->Hide(); });
        }
    }
    catch (const std::out_of_range& exception)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Tooltip group not found.");
        UmLogger.Log(LogLevel::LEVEL_WARNING, exception.what());
    }
}

TooltipComponent::TooltipData TooltipSystem::GetTooltip(const int id) const
{
    try
    {
        return _tooltips.at(id);
    }
    catch (const std::out_of_range& exception)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Tooltip data not found.");
        UmLogger.Log(LogLevel::LEVEL_WARNING, exception.what());
        return TooltipComponent::TooltipData{};
    }
}

void TooltipSystem::Awake()
{
    Component::Awake();

    if (_singletonObject.TrySingleTon(true))
    {
        _singletonComponent.TrySingleTon();
    }

    FindComponents();
}

void TooltipSystem::Start()
{
    Component::Start();

    SetupData();
}

void TooltipSystem::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    ShowDataProperty();
    ShowTestTooltipProperty();
}

void TooltipSystem::FindComponents()
{
    _tooltipGroups.emplace(Group::WEAPON,
                           GameObject::FindComponentWithTag<TooltipGroupComponent>(TOOLTIP_GROUP_WEAPON.data()));
    _tooltipGroups.emplace(Group::ACCESSORY,
                           GameObject::FindComponentWithTag<TooltipGroupComponent>(TOOLTIP_GROUP_ACCESSORY.data()));
    _tooltipGroups.emplace(Group::PLAYER,
                           GameObject::FindComponentWithTag<TooltipGroupComponent>(TOOLTIP_GROUP_PLAYER.data()));
    _tooltipGroups.emplace(Group::ENEMY_LEFT,
                           GameObject::FindComponentWithTag<TooltipGroupComponent>(TOOLTIP_GROUP_ENEMY_LEFT.data()));
    _tooltipGroups.emplace(Group::ENEMY_MIDDLE,
                           GameObject::FindComponentWithTag<TooltipGroupComponent>(TOOLTIP_GROUP_ENEMY_MIDDLE.data()));
    _tooltipGroups.emplace(Group::ENEMY_RIGHT,
                           GameObject::FindComponentWithTag<TooltipGroupComponent>(TOOLTIP_GROUP_ENEMY_RIGHT.data()));
    _tooltipGroups.emplace(Group::REVELATION_UP,
                           GameObject::FindComponentWithTag<TooltipGroupComponent>(TOOLTIP_GROUP_REVELATION_UP.data()));
    _tooltipGroups.emplace(Group::REVELATION_MIDDLE, GameObject::FindComponentWithTag<TooltipGroupComponent>(
                                                         TOOLTIP_GROUP_REVELATION_MIDDLE.data()));
    _tooltipGroups.emplace(Group::REVELATION_DOWN,
                            GameObject::FindComponentWithTag<TooltipGroupComponent>(
                                                       TOOLTIP_GROUP_REVELATION_DOWN.data()));
}

void TooltipSystem::SetupData()
{
    if (ExcelDataSystem* excelDataSystemComponent = SingletonComponent<ExcelDataSystem>::GetInstance())
    {
        if (const std::unique_ptr<ExcelDataBase> tooltipSheet = excelDataSystemComponent->FindExcelDataBase(DATA_TOOLTIP_SHEET_NAME))
        {
            if (const std::unique_ptr<ExcelDataBase> textSheet =
                    excelDataSystemComponent->FindExcelDataBase(DATA_TEXT_SHEET_NAME))
            {
                const size_t rowCount = tooltipSheet->RowCount();
                for (size_t row = 0; row < rowCount; ++row)
                {
                    std::string_view idStringView = tooltipSheet->FindData(row, DATA_TOOLTIP_COLUMN_KEY_ID);
                    std::string      idString     = std::string(idStringView);
                    int              id           = idString.empty() ? 0 : std::stoi(idString);

                    std::string_view nameIdStringView = tooltipSheet->FindData(row, DATA_TOOLTIP_COLUMN_KEY_NAME_ID);
                    std::u8string    nameIdU8String =
                        std::u8string(reinterpret_cast<const char8_t*>(nameIdStringView.data()));
                    const size_t     nameIndex       = textSheet->FindRowIndex(nameIdU8String, DATA_TEXT_COLUMN_KEY_ID);
                    std::string_view nameContentView = textSheet->FindData(nameIndex, DATA_TEXT_COLUMN_CONTENT);
                    std::string      nameContent     = std::string(nameContentView);

                    std::string_view descriptionIdStringView =
                        tooltipSheet->FindData(row, DATA_TOOLTIP_COLUMN_KEY_DESCRIPTION_ID);
                    std::u8string descriptionIdU8String =
                        std::u8string(reinterpret_cast<const char8_t*>(descriptionIdStringView.data()));
                    const size_t descriptionIndex =
                        textSheet->FindRowIndex(descriptionIdU8String, DATA_TEXT_COLUMN_KEY_ID);
                    std::string_view descriptionContentView =
                        textSheet->FindData(descriptionIndex, DATA_TEXT_COLUMN_CONTENT);
                    std::string descriptionContent = std::string(descriptionContentView);

                    std::string_view iconIdStringView = tooltipSheet->FindData(row, DATA_TOOLTIP_COLUMN_KEY_ICON_ID);
                    std::string      iconIdString     = std::string(iconIdStringView);
                    int              iconId           = iconIdString.empty() ? 0 : std::stoi(iconIdString);

                    TooltipComponent::TooltipData tooltipData{.ImageAssetId = iconId,
                                                              .Title        = std::move(nameContent),
                                                              .Description  = std::move(descriptionContent)};

                    _tooltips.emplace(id, std::move(tooltipData));
                }
            }
        }
    }
}

void TooltipSystem::ShowDataProperty()
{
    if (ImGui::TreeNodeEx("Tooltip Data##details"))
    {
        if (ImGui::BeginTable("TooltipTable##Details", 4, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
        {
            // Headers
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("Image AssetID", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("Description Existed", ImGuiTableColumnFlags_WidthStretch, 0.1f);
            ImGui::TableHeadersRow();

            for (const auto& [id, tooltips] : _tooltips)
            {
                ImGui::PushID(id);
                ImGui::TableNextRow();
                // ID
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", id);
                // Title
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(tooltips.Title.c_str());
                // Image AssetID
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%d", tooltips.ImageAssetId);
                // Description Existed
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%s", tooltips.Description.empty() ? "No" : "Yes");
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
}

void TooltipSystem::ShowTestTooltipProperty()
{
    constexpr const char* groupItems[] = {"Weapon",        "Accessory",         "Player",
                                          "Enemy Left",    "Enemy Middle",      "Enemy Right",
                                          "Revelation Up", "Revelation Middle", "Revelation Down"};

    static int id = 0;
    static Group group = Group::WEAPON;

    ImGui::InputInt("Tooltip ID", &id);
    if (ImGui::BeginCombo("Tooltip Group", groupItems[static_cast<size_t>(group)]))
    {
        for (size_t i = 0; i < std::size(groupItems); ++i)
        {
            const bool isSelected = (static_cast<size_t>(group) == i);
            if (ImGui::Selectable(groupItems[i], isSelected))
            {
                group = static_cast<Group>(i);
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Show Tooltip"))
    {
        Show(group, id);
    }

    if (ImGui::Button("Hide Tooltip"))
    {
        Hide(group);
    }
}
