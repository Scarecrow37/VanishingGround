#pragma once

class ImageElement;

class TurnQueueView : public Component
{
    USING_PROPERTY(Component)

public:
    TurnQueueView();

protected:
    void Reset() override;

private:
    static void FindImageElementWithTag(const std::string& tag, std::array<ImageElement*, 7>& elements, size_t index);
    void        FindFramesWithTag(const std::string& tag, size_t index);
    void        FindPortraitsWithTag(const std::string& tag, size_t index);
    void        InitializeFramesAndPortraits();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnQueueView)

private:
    std::array<ImageElement*, 7> _turnQueueFrames;
    std::array<ImageElement*, 7> _turnQueuePortraits;
};
