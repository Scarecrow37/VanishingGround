#pragma once
#include "TooltipType.h"

class ImageElement;
class TextElement;
class DescriptionPanel;

class TooltipComponent : public Component
{
    USING_PROPERTY(TooltipComponent)

    static constexpr const char* TAG_PANEL_OBJECT          = "Tooltip Panel";
    static constexpr const char* TAG_IMAGE_COMPONENT       = "Tooltip Image";
    static constexpr const char* TAG_TITLE_COMPONENT       = "Tooltip Title";
    static constexpr const char* TAG_DESCRIPTION_COMPONENT = "Tooltip Description";

public:


public:
    TooltipComponent();

public:
    REFLECT_PROPERTY()

public:
    void SetTooltip(const Tooltip::TooltipData& data) const;
    void Show() const;
    void Hide() const;

protected:
    void Awake() override;

    void ImGuiDrawPropertysEvent() override;

private:
    void FindComponents();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TooltipComponent)

private:
    std::weak_ptr<ImageElement>     _image;
    std::weak_ptr<TextElement>      _title;
    std::weak_ptr<DescriptionPanel> _description;
};