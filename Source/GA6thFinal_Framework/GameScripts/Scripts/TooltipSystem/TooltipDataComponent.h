#pragma once
#include "TooltipSystem.h"

class TooltipDataComponent : public Component
{
    USING_PROPERTY(TooltipDataComponent)

public:
    TooltipDataComponent();

public:
    REFLECT_PROPERTY()

    GETTER(TooltipSystem::Group, Group) { return ReflectFields->Group; }
    SETTER(TooltipSystem::Group, Group) { ReflectFields->Group = value; }
    PROPERTY(Group)

public:
    void SetTooltips(std::initializer_list<int> ids);
    std::span<const int> GetTooltips() const;

protected:
    void ImGuiDrawPropertysEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    TooltipSystem::Group Group = TooltipSystem::Group::WEAPON;
    REFLECT_FIELDS_END(TooltipDataComponent)

private:
    std::vector<int> _tooltipIds;
};