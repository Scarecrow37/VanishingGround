#include "pchScripts.h"
#include "TooltipGroupComponent.h"
#include "Scripts/UI/Contents/TooltipColumnComponent.h"
#include "Scripts/UI/Animations/FadeUIComponent/FadeUIComponent.h"
#include "TooltipSystem/TooltipSystem.h"
#include "Utility/SingletonHelper.h"

class TooltipSystem;
UMREAL_COMPONENT(TooltipGroupComponent)

struct GetSecondary
{
    TooltipGroupComponent::ColumnType operator()(const TooltipGroupComponent::ColumnType primary) const
    {
        using ColumnType = TooltipGroupComponent::ColumnType;
        switch (primary)
        {
        case ColumnType::LEFT:
            return ColumnType::RIGHT;
        case ColumnType::RIGHT:
            return ColumnType::LEFT;
        default:
            throw std::out_of_range("Invalid ColumnType.");
        }
    }
};

TooltipGroupComponent::TooltipGroupComponent() = default;

void TooltipGroupComponent::Show(const int tooltipId, const Tooltip::TooltipData& data)
{
    if (auto [_, succeed] = _activeTooltipIds.insert(tooltipId); succeed)
    {
        Show(data);
    }
}

void TooltipGroupComponent::Show(const Tooltip::TooltipData& data) const
{
    const ColumnType primaryColumn = PrimaryColumn;
    try
    {
        std::shared_ptr<TooltipColumnComponent> targetColumn = nullptr;
        if (const auto sharedPrimaryColumn = _columns.at(primaryColumn).lock();
            nullptr != sharedPrimaryColumn && false == sharedPrimaryColumn->IsFull())
        {
            targetColumn = sharedPrimaryColumn;
        }
        else if (const auto sharedSecondaryColumn = _columns.at(GetSecondary()(primaryColumn)).lock();
                 nullptr != sharedSecondaryColumn && false == sharedSecondaryColumn->IsFull())
        {
            targetColumn = sharedSecondaryColumn;
        }

        if (nullptr != targetColumn)
        {
            targetColumn->Show(data);
        }
    }
    catch (const std::out_of_range&)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Fail to show tooltip. Tooltip Column is not existed.");
    }
}

void TooltipGroupComponent::Hide()
{
    if (_isFadingOut)
    {
        for (auto weakColumnComponent : _columns | std::views::values)
        {
            if (const auto sharedColumnComponent = weakColumnComponent.lock())
            {
                sharedColumnComponent->Hide();
            }
        }
        _activeTooltipIds.clear();
    }
}

void TooltipGroupComponent::FadeIn()
{
    if (const auto fadeUI = _fadeUI.lock())
    {
        fadeUI->FadeIn();
    }
    _isFadingOut = false;
}

void TooltipGroupComponent::FadeOut()
{
    if (const auto fadeUI = _fadeUI.lock())
    {
        fadeUI->FadeOut();
    }
    _isFadingOut = true;
}

float TooltipGroupComponent::GetFadeDuration() const
{
    if (const auto fadeUI = _fadeUI.lock())
    {
        return fadeUI->FadeDuration;
    }
    return 0.0f;
}

bool TooltipGroupComponent::IsFadingOut() const
{
    return _isFadingOut;
}

void TooltipGroupComponent::Awake()
{
    Component::Awake();

    FindComponents();
}

void TooltipGroupComponent::Start()
{
    Component::Start();

    if (TooltipSystem* system = SingletonComponent<TooltipSystem>::GetInstance())
    {
        system->RegisterTooltipGroup(ReflectFields->Group, GetWeakPtrAs<TooltipGroupComponent>());
    }

    Hide();
}

void TooltipGroupComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    static Tooltip::TooltipData data = {};
    ImGui::InputInt("Image Asset Id", &data.ImageAssetId);
    ImGui::InputText("Title", &data.Title);
    ImGui::InputText("Description", &data.Description);

    if (ImGui::Button("Show Tooltip"))
    {
        Show(data);
    }

    if (ImGui::Button("Hide Tooltip"))
    {
        Hide();
    }
}

void TooltipGroupComponent::FindComponents()
{
    _columns.reserve(MAX_COLUMN_COUNT);

    Transform::ForeachBFS(transform, [this](const Transform* childTransform, const int depth) {
        if (depth == 1 && _columns.size() < MAX_COLUMN_COUNT)
        {
            if (const TooltipColumnComponent* tooltipColumnComponent =
                    childTransform->gameObject->GetComponentDynamic<TooltipColumnComponent>())
            {
                std::weak_ptr<TooltipColumnComponent> weakPtr =
                    tooltipColumnComponent->GetWeakPtrAs<TooltipColumnComponent>();
                if (false == _columns.contains(ColumnType::LEFT))
                {
                    _columns.emplace(ColumnType::LEFT, weakPtr);
                }
                else
                {
                    _columns.emplace(ColumnType::RIGHT, weakPtr);
                }
            }
        }
    });

    if (const FadeUIComponent* fadeUi = gameObject->GetComponent<FadeUIComponent>())
    {
        _fadeUI = fadeUi->GetWeakPtrAs<FadeUIComponent>();
    }
}

