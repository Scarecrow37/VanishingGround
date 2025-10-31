#pragma once

class TextElement;
class DescriptionPanel;

class TooltipComponent : public Component
{
    USING_PROPERTY(TooltipComponent)

    static constexpr const char* TAG_PANEL_OBJECT          = "Tooltip Panel";
    static constexpr const char* TAG_TITLE_COMPONENT       = "Tooltip Title";
    static constexpr const char* TAG_DESCRIPTION_COMPONENT = "Tooltip Description";

public:
    struct TooltipData
    {
        std::string Title;
        std::string Description;
        Color TitleColor;
    };

public:
    TooltipComponent();

public:
    REFLECT_PROPERTY()

public:
    void SetTooltip(const TooltipData& data) const;
    void ShowTooltip() const;
    void HideTooltip() const;

protected:
    void Awake() override;
    void Start() override;

    void ImGuiDrawPropertysEvent() override;

private:
    void FindComponent();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TooltipComponent)

private:
    std::weak_ptr<GameObject>       _panel;
    std::weak_ptr<TextElement>      _title;
    std::weak_ptr<DescriptionPanel> _description;
};