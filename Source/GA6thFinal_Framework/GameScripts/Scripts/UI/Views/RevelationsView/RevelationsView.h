#pragma once
#include "ViewModels/Revelations/RevelationsViewModel.h"

class ImageElement;
class TextElement;
class DescriptionPanel;
class FadeUIComponent;
class SpriteAnimationElement;

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
    void                                 ClearRevelationUIs();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(RevelationsView)

private:
    std::array<RevelationUI, 3> _revelationUis{};
    std::array<GameObject*, 3>  _revelationObjects{};

    std::weak_ptr<FadeUIComponent> _rootFade;
    std::weak_ptr<FadeUIComponent> _textsFade;

    std::weak_ptr<SpriteAnimationElement> _startAnimation;
    std::weak_ptr<SpriteAnimationElement> _reloadAnimation;

    RevelationsViewModel::Handle _watchHandle;
};
