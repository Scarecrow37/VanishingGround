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

private:
    ImageElement* FindImageElementWithTag(const std::string& tag) const;
    void InitializeFramesAndPortraits();
    void          FindFramesWithTag(const std::string& tag);
    void          FindPortraitsWithTag(const std::string& tag);

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnQueueView)

private:
    std::array<ImageElement*, 7> _turnQueueFrames{};
    std::array<ImageElement*, 7> _turnQueuePortraits{};

    TurnQueueViewModel::Handle _watchHandle;
};
