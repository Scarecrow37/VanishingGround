#pragma once
#include "Scripts/UI/Contents/TooltipComponent.h"

class TooltipColumnComponent;

class TooltipGroupComponent : public Component
{
    USING_PROPERTY(TooltipGroupComponent)

    static constexpr size_t MAX_COLUMN_COUNT = 2;

    enum class ColumnType
    {
        LEFT,
        RIGHT
    };

public:
    TooltipGroupComponent();

public:
    REFLECT_PROPERTY(PrimaryColumn)

    GETTER(ColumnType, PrimaryColumn) { return ReflectFields->PrimaryColumn; }
    SETTER(ColumnType, PrimaryColumn) { ReflectFields->PrimaryColumn = value; }
    PROPERTY(PrimaryColumn)

public:
    void Show(const TooltipComponent::TooltipData& data);
    void Hide();

protected:
    void Awake() override;

    void ImGuiDrawPropertysEvent() override;

private:
    void FindComponent();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    ColumnType PrimaryColumn = ColumnType::LEFT;
    REFLECT_FIELDS_END(TooltipGroupComponent)

private:
    std::unordered_map<ColumnType, std::weak_ptr<TooltipColumnComponent>> _columns;
};