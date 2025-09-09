#pragma once
#include "UmFramework.h"
class SceneTransitionComponent : public Component
{
public:
    SceneTransitionComponent();
    ~SceneTransitionComponent() override;

    USING_PROPERTY(SceneTransitionComponent)
public:
    REFLECT_PROPERTY(Duration, Maintain)

    GETTER(float, Duration) { return ReflectFields->Duration; }
    SETTER(float, Duration)
    {
        ReflectFields->Duration = value;
    }
    PROPERTY(Duration)

    GETTER(float, Maintain) { return ReflectFields->Maintain; }
    SETTER(float, Maintain)
    {
        ReflectFields->Maintain = value;
    }
    PROPERTY(Maintain)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::array<float, 4> StartColorArray;
    std::array<float, 4> EndColorArray;
    float                Duration;
    float                Maintain;
    REFLECT_FIELDS_END(SceneTransitionComponent)

    Vector4 _startColor{0, 0, 0, 0};
    Vector4 _endColor{0, 0, 0, 0};

    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

public:
    void Fade(float duration, float maintain, const Vector4& start, const Vector4& end);
};
