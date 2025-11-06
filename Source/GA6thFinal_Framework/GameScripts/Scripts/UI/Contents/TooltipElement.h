#pragma once
#include "UI/Panels/Horizontal/HorizontalPanel.h"

class DescriptionPanel;
class TextElement;
class ImageElement;

class TooltipElement : public HorizontalPanel
{
    USING_PROPERTY(TooltipElement)

public:
    TooltipElement();

public:
    REFLECT_PROPERTY()

public:
    void FindComponents();

    void SetTooltip(int id) const;

protected:
    void ImGuiDrawPropertysEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(HorizontalPanel)
    REFLECT_FIELDS_END(TooltipElement)

private:
    std::weak_ptr<ImageElement>     _icon;
    std::weak_ptr<TextElement>      _name;
    std::weak_ptr<DescriptionPanel> _description;
};