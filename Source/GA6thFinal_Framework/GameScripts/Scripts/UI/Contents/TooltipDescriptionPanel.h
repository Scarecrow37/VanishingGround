#pragma once
#include "UI/Panels/Horizontal/HorizontalPanel.h"

class ImageElement;
class TextElement;
class DescriptionPanel;

class TooltipDescriptionPanel : public HorizontalPanel
{
    USING_PROPERTY(TooltipDescriptionPanel)

    using TooltipElements = std::tuple<std::weak_ptr<ImageElement>, std::weak_ptr<TextElement>, std::weak_ptr<DescriptionPanel>>;

public:
    TooltipDescriptionPanel();

public:
    REFLECT_PROPERTY()

public:
    void AddTooltip(int id);
    void SetTooltips(std::span<const int> ids);
    void SetTooltips(std::initializer_list<int> ids);

private:
    TooltipElements CreateTooltipElements();
    void ClearTooltips();

protected:
    REFLECT_FIELDS_BEGIN(HorizontalPanel)
    float TooltipHeight = 24.0f;
    float TooltipSpace  = 4.0f;
    REFLECT_FIELDS_END(TooltipDescriptionPanel)
};
