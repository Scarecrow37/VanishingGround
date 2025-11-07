#pragma once
#include "Scripts/UI/Contents/TooltipType.h"

class TooltipComponent;

class TooltipColumnComponent : public Component
{
    USING_PROPERTY(TooltipColumnComponent)

    static constexpr size_t MAX_TOOLTIP_COUNT = 4;

    struct TooltipAndActive
    {
        std::weak_ptr<TooltipComponent> Component;
        bool                            IsActive;
    };

public:
    TooltipColumnComponent();

public:
    REFLECT_PROPERTY()

public:
    bool IsFull() const;
    void Show(const Tooltip::TooltipData& data);
    void Hide();

protected:
    void Awake() override;

    void ImGuiDrawPropertysEvent() override;

private:
    void FindComponent();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TooltipColumnComponent)

private:
    std::vector<TooltipAndActive> _tooltips;
};