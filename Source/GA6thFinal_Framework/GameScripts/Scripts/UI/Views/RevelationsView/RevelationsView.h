#pragma once
#include "ViewModels/Revelations/RevelationsViewModel.h"

class ImageElement;
class TextElement;
class DescriptionPanel;

struct RevelationUI
{
    ImageElement* IconElement;
    std::vector<ImageElement*> GradeElements;
    TextElement*  NameElement;
    DescriptionPanel* DescriptionElement;
};

class RevelationsView : public Component
{
    USING_PROPERTY(RevelationsView)

public:
    RevelationsView() = default;
    virtual ~RevelationsView();

public:
    const std::array<RevelationUI, 3>& GetRevelationUIs() const { return _revelationUis; }
    const std::array<GameObject*, 3>& GetRevelationObjects() const { return _revelationObjects; }

protected:
    void Awake() override;
    void Start() override;
    void OnDestroy() override;

private:
    void FindRevelationUIs();
    std::pair<GameObject*, RevelationUI> FindRevelationUI(const std::string& tag) const;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(RevelationsView)

private:
    std::array<RevelationUI, 3> _revelationUis{};
    std::array<GameObject*, 3>  _revelationObjects{};

    RevelationsViewModel::Handle _watchHandle;
};
