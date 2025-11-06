#pragma once

class FadeImageElement;
class FadeTextElement;
class RoundInfoUIManager : public Component
{
    USING_PROPERTY(RoundInfoUIManager)

public:
    RoundInfoUIManager();
    ~RoundInfoUIManager() override;

public:
    REFLECT_PROPERTY(
        ShowUITime,
        Duration
    )

    GETTER(float, ShowUITime) { return ReflectFields->ShowUITime; }
    SETTER(float, ShowUITime) { ReflectFields->ShowUITime = value; }
    PROPERTY(ShowUITime)

    GETTER(float, Duration) { return ReflectFields->Duration; }
    SETTER(float, Duration) { ReflectFields->Duration = value; }
    PROPERTY(Duration)

    GETTER_ONLY(float, UIAnimationTime) { return ShowUITime + Duration; }
    // UI 페이드의 총 시간 입니다.
    PROPERTY(UIAnimationTime)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float ShowUITime = 1.f;
    float Duration   = 1.f;
    REFLECT_FIELDS_END(RoundInfoUIManager)

protected:
    void Awake() override;

public:
    void FadeInfoUI(std::string_view text);

private:
    void FindUIElements();

private:
    std::weak_ptr<FadeImageElement> _fadeImageElement;
    std::weak_ptr<FadeTextElement>  _fadeTextElement;
  
};

