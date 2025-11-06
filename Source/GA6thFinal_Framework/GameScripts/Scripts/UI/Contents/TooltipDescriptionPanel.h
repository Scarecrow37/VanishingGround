#pragma once
#include "UI/Panels/Horizontal/HorizontalPanel.h"

class TooltipElement;

class TooltipDescriptionPanel : public HorizontalPanel
{
    USING_PROPERTY(TooltipDescriptionPanel)

public:
    TooltipDescriptionPanel();

public:
    REFLECT_PROPERTY(TooltipElementPrefab)

    GETTER_ONLY(std::string, TooltipElementPrefab)
    {
        return File::Guid(ReflectFields->TooltipElementPrefabGuid).ToPath().string();
    }
    PROPERTY(TooltipElementPrefab)

public:
    void SetTooltip(int id);
    void SetTooltips(std::initializer_list<int> ids);
    void SetTooltips(std::span<const int> ids);

protected:
    void Reset() override;
    void ImGuiDrawPropertysEvent() override;

private:
    std::weak_ptr<TooltipElement> CreateTooltipElements();
    void                          ClearTooltips() const;

protected:
    REFLECT_FIELDS_BEGIN(HorizontalPanel)
    std::string TooltipElementPrefabGuid;
    REFLECT_FIELDS_END(TooltipDescriptionPanel)
};
