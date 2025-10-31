#pragma once

class TooltipComponent;

class TooltipColumnComponent : public Component
{
    USING_PROPERTY(TooltipColumnComponent)

    struct Tooltip
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
    void Hide();

protected:
    void Awake() override;

private:
    void FindComponent();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TooltipColumnComponent)

private:
    std::vector<Tooltip> _tooltips;
};