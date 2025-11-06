#pragma once
#include "Scripts/UI/Contents/TooltipComponent.h"

class FadeUIComponent;
class TooltipColumnComponent;

class TooltipGroupComponent : public Component
{
    USING_PROPERTY(TooltipGroupComponent)

    static constexpr size_t MAX_COLUMN_COUNT = 2;

public:
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
    void Show(int tooltipId, const Tooltip::TooltipData& data);
    void Show(const Tooltip::TooltipData& data) const;
    void Hide();

public:
    void FadeIn();
    void FadeOut();
    float GetFadeDuration() const;
    bool  IsFadingOut() const;

protected:
    void Awake() override;
    void Start() override;

    void ImGuiDrawPropertysEvent() override;

private:
    void FindComponents();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    ColumnType PrimaryColumn = ColumnType::LEFT;
    REFLECT_FIELDS_END(TooltipGroupComponent)

private:
    std::unordered_map<ColumnType, std::weak_ptr<TooltipColumnComponent>> _columns;

    std::weak_ptr<FadeUIComponent> _fadeUI;

    std::unordered_set<int> _activeTooltipIds;

    bool _isFadingOut = true;
};