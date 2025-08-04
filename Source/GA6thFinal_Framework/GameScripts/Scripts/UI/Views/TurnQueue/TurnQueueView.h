#pragma once

class ImageElement;

class TurnQueueView : public Component
{
    USING_PROPERTY(Component)

public:
    TurnQueueView();
    ~TurnQueueView() override;

protected:
    void Awake() override;
    void Start() override;

private:
    ImageElement* FindImageElementWithTag(const std::string& tag) const;
    void FindFramesWithTag(const std::string& tag, size_t index);
    void FindPortraitsWithTag(const std::string& tag, size_t index);
    void InitializeFramesAndPortraits();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnQueueView)

private:
    std::array<ImageElement*, 7> _turnQueueFrames;
    ImageElement*                _firstTurnQueueFrameLeftWing;
    ImageElement*                _firstTurnQueueFrameRightWing;
    std::array<ImageElement*, 7> _turnQueuePortraits;
};
