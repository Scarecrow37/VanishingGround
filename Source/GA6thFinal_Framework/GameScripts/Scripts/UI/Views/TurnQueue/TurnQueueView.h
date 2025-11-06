#pragma once
#include "ViewModels/TurnQueue/TurnQueueViewModel.h"

class ImageElement;
class FadeUIComponent;

class TurnQueueView : public Component
{
    USING_PROPERTY(TurnQueueView)

public:
    struct ButtonIconImage
    {
        ImageElement* X = nullptr;
        ImageElement* Y = nullptr;
        ImageElement* B = nullptr;
    };
    using ImageElementList = std::array<ImageElement*, 6>;
    using ButtonImageList  = std::array<ButtonIconImage, 6>;

    TurnQueueView();
    ~TurnQueueView() override;

public:
    REFLECT_PROPERTY()

    GETTER_ONLY(GameObject*, TurnQueueHorizontalPanel) { return _turnQueueHorizontalPanel; }
    PROPERTY(TurnQueueHorizontalPanel)
    GETTER_ONLY(const ImageElementList&, TurnQueueFrames) { return _turnQueueFrames; }
    PROPERTY(TurnQueueFrames)
    GETTER_ONLY(const ImageElementList&, TurnQueuePortraits) { return _turnQueuePortraits; }
    PROPERTY(TurnQueuePortraits)
    GETTER_ONLY(const ButtonImageList&, TurnQueueButtonIcons) { return _turnQueueButtonIcons; }
    PROPERTY(TurnQueueButtonIcons)

protected:
    void ImGuiDrawPropertysEvent() override;
    void Awake() override;
    void Start() override;
    void OnDestroy() override;

private:
    ImageElement* FindImageElementWithTag(const std::string& tag) const;
    void          InitializeFramesAndPortraits();
    void          FindFramesWithTag(const std::string& tag);
    void          FindPortraitsWithTag(const std::string& tag);
    void          FindButtonIconsWithTag(const std::string& tag);

    void          DisableButtonIcons();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnQueueView)

private:
    GameObject*         _turnQueueHorizontalPanel = nullptr;
    ImageElementList    _turnQueueFrames{};
    ImageElementList    _turnQueuePortraits{};
    
    std::array<ButtonIconImage, 6> _turnQueueButtonIcons{};
    TurnQueueViewModel::Handle _watchHandle;

    std::weak_ptr<FadeUIComponent> _fadeUIComponent;

private:
    void DisableButtonIcons(ButtonIconImage& image);
    void UpdateButtonIcons(ButtonIconImage& image, TurnUIData::ActorType type);
};
