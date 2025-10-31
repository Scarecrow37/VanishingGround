#include "pchScripts.h"
#include "TooltipGroupComponent.h"
#include "Scripts/UI/Contents/TooltipColumnComponent.h"

UMREAL_COMPONENT(TooltipGroupComponent)

TooltipGroupComponent::TooltipGroupComponent() = default;

void TooltipGroupComponent::Show(const TooltipComponent::TooltipData& data)
{
    ColumnType primaryColumn = PrimaryColumn;
    try
    {
        auto weakTooltipColumnComponent = _columns.at(primaryColumn);

    }
    catch (const std::out_of_range& exception)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, "Fail to show tooltip. Tooltip Column is not existed.");
    }
}

void TooltipGroupComponent::Hide()
{
    for (auto weakColumnComponent : _columns | std::views::values )
    {
        if (const auto sharedColumnComponent = weakColumnComponent.lock())
        {
            sharedColumnComponent->Hide();
        }
    }
}

void TooltipGroupComponent::Awake()
{
    Component::Awake();

    FindComponent();
}

void TooltipGroupComponent::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();
}

void TooltipGroupComponent::FindComponent()
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
}

