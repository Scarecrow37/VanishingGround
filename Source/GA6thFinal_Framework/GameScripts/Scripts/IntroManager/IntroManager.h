#pragma once

class FadeTextElement;
class FadeDescriptionPanel;

class IntroManager : public Component
{
    USING_PROPERTY(IntroManager)

    enum Step
    {
        WAIT_INTRO_DESCRIPTION,
        FADE_IN_INTRO_DESCRIPTION,
        WAIT_LEVEL_SELECTION,
        FADE_IN_LEVEL_SELECTION,
        WAIT_PROMPT,
        FADE_IN_PROMPT,
        END
    };

public:
    IntroManager();

public:
    REFLECT_PROPERTY(FadeDuration, DescriptionDelay, LevelSelectDelay, PromptDelay)

    GETTER(float, FadeDuration) { return ReflectFields->FadeDuration; }
    SETTER(float, FadeDuration) { ReflectFields->FadeDuration = std::max(0.1f, value); }
    PROPERTY(FadeDuration)

    GETTER(float, DescriptionDelay) { return ReflectFields->DescriptionDelay; }
    SETTER(float, DescriptionDelay) { ReflectFields->DescriptionDelay = std::max(0.0f, value); }
    PROPERTY(DescriptionDelay)

    GETTER(float, LevelSelectDelay) { return ReflectFields->LevelSelectDelay; }
    SETTER(float, LevelSelectDelay) { ReflectFields->LevelSelectDelay = std::max(0.0f, value); }
    PROPERTY(LevelSelectDelay)

    GETTER(float, PromptDelay) { return ReflectFields->PromptDelay; }
    SETTER(float, PromptDelay) { ReflectFields->PromptDelay = std::max(0.0f, value); }
    PROPERTY(PromptDelay)

protected:
    void Start() override;

    void Update() override;

    void Reset() override;

    void ImGuiDrawPropertysEvent() override;

private:
    float            GetWaitDescriptionTime() const;
    float            GetFadeDescriptionTime() const;
    float            GetWaitLevelSelectionTime() const;
    float            GetFadeLevelSelectionTime() const;
    float            GetWaitPromptTime() const;
    float            GetFadePromptTime() const;
    FadeTextElement* GetFadeTextElement(const std::string& tag) const;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float FadeDuration = 1.0f;
    float DescriptionDelay = 2.0f;
    float LevelSelectDelay = 2.0f;
    float PromptDelay      = 1.0f;
    REFLECT_FIELDS_END(IntroManager)

private:
    Step  _step;
    float _elapsedTime = 0.0f;
    bool  _isLevelSelected;

    FadeDescriptionPanel* _introDescription;
    FadeTextElement*      _normalLevelText;
    FadeTextElement*      _hardLevelText;
    FadeTextElement*      _promptText;
};