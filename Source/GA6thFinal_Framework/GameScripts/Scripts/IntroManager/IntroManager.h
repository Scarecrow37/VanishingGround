#pragma once

class FadeDescriptionPanel;

class IntroManager : public Component
{
    USING_PROPERTY(IntroManager)

    enum Step
    {
        WAIT_INTRO_DESCRIPTION,
        FADE_IN_INTRO_DESCRIPTION,
        END
    };

public:
    IntroManager();

public:
    REFLECT_PROPERTY()

    GETTER(float, IntroDescriptionDelay) { return ReflectFields->IntroDescriptionDelay; }
    SETTER(float, IntroDescriptionDelay) { ReflectFields->IntroDescriptionDelay = std::max(0.0f, value); }
    PROPERTY(IntroDescriptionDelay)

protected:
    void Start() override;

    void Update() override;

    void Reset() override;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float IntroDescriptionDelay = 2.0f;
    REFLECT_FIELDS_END(IntroManager)

private:
    Step                  _step;
    FadeDescriptionPanel* _introDescription;
    float                 _elapsedTime = 0.0f;
};