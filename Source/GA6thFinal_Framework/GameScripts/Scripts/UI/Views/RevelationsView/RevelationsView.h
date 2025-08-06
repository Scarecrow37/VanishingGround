#pragma once

class ImageElement;
class TextElement;
class DescriptionPanel;

struct RevelationUI
{
    ImageElement* IconElement;
    TextElement*  NameElement;
    DescriptionPanel* Description1Element;
    DescriptionPanel* Description2Element;
};

class RevelationsView : public Component
{
    USING_PROPERTY(RevelationsView)

public:
    RevelationsView() = default;
    virtual ~RevelationsView();

protected:
    void Awake() override;
    void Start() override;

private:
    void FindRevelationUIs();
    std::pair<GameObject*, RevelationUI> FindRevelationUI(const std::string& tag) const;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(RevelationsView)

private:
    std::array<RevelationUI, 3> _revelationUis;
    std::array<GameObject*, 3>  _revelationObjects;
};
