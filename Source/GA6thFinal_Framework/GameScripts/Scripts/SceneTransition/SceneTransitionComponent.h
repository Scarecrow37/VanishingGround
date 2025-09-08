#pragma once
#include "UmFramework.h"
class SceneTransitionComponent : public Component
{

public:
    SceneTransitionComponent();
    ~SceneTransitionComponent() override;
    USING_PROPERTY(SceneTransitionComponent)
public:
    REFLECT_PROPERTY(Duration)

    
    GETTER(float, Duration) { return ReflectFields->Duration; }
    SETTER(float, Duration)
    {
        ReflectFields->Duration = value;
        isDirty                 = true;

    }
    PROPERTY(Duration)


protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::array<float, 4> StartColorArray;
    std::array<float, 4> EndColorArray;
    float                Duration;
    REFLECT_FIELDS_END(SceneTransitionComponent)

    Vector4 _startColor{0, 0, 0, 0};
    Vector4 _endColor{0, 0, 0, 0};
    bool    isDirty = false;

    void Update() override;
    void Start() override;
    void Reset() override;
    void ImGuiDrawPropertysEvent() override;

        void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;



    void Fade(float duration, const Vector4& start, const Vector4& end);



};
