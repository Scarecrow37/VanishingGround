#pragma once
#include "ViewModels/TurnQueue/TurnQueueViewModel.h"

class ImageElement;

class TurnQueueView : public Component
{
    USING_PROPERTY(TurnQueueView)

public:
    TurnQueueView();
    ~TurnQueueView() override;

protected:
    void ImGuiDrawPropertysEvent() override;
    void Awake() override;
    void Start() override;
    void OnDestroy() override;

private:
    ImageElement* FindImageElementWithTag(const std::string& tag) const;
    void InitializeFramesAndPortraits();
    void          FindFramesWithTag(const std::string& tag);
    void          FindPortraitsWithTag(const std::string& tag);
    void          FindButtonIconsWithTag(const std::string& tag);

    void          DisableButtonIcons();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnQueueView)

private:
    GameObject*                  _turnQueueHorizontalPanenl = nullptr;
    std::array<ImageElement*, 6> _turnQueueFrames{};
    std::array<ImageElement*, 6> _turnQueuePortraits{};
    struct ButtonIconImage
    {
        ImageElement* X = nullptr;
        ImageElement* Y = nullptr;
        ImageElement* B = nullptr;
    };
    std::array<ButtonIconImage, 6> _turnQueueButtonIcons{};
    TurnQueueViewModel::Handle _watchHandle;

private:
    void DisableButtonIcons(ButtonIconImage& image);
    void UpdateButtonIcons(ButtonIconImage& image, TurnUIData::ActorType type);
};
