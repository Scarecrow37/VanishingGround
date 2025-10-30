#pragma once
#include "UI/Elements/Image/ImageElement.h"

class SpriteAnimationElement : public ImageElement
{
    USING_PROPERTY(SpriteAnimationElement)

public:
    REFLECT_PROPERTY(Loop, Duration, EmptyFrameCount, StartAnimationOnPlay)

    GETTER(bool, Loop) { return ReflectFields->Loop; }
    SETTER(bool, Loop) { ReflectFields->Loop = value; }
    PROPERTY(Loop)

    GETTER(float, Duration) { return ReflectFields->Duration; }
    SETTER(float, Duration)
    {
        ReflectFields->Duration = std::max(0.01f, value);
    }
    PROPERTY(Duration)

    GETTER_ONLY(int, GridCount) { return ReflectFields->Basefields.get().Column * ReflectFields->Basefields.get().Row; }
    PROPERTY(GridCount)

    GETTER(int, EmptyFrameCount) { return ReflectFields->EmptyFrameCount; }
    SETTER(int, EmptyFrameCount)
    {
        const int gridCount = GridCount;
        ReflectFields->EmptyFrameCount = std::clamp(value, 0, gridCount - 1);
    }
    PROPERTY(EmptyFrameCount)

    GETTER_ONLY(int, FrameCount) { return GridCount - EmptyFrameCount; }
    PROPERTY(FrameCount)

    GETTER_ONLY(bool, IsPlaying) { return _isPlaying; }
    PROPERTY(IsPlaying)

    GETTER(bool, WillSuicide) { return _willSuicide; }
    SETTER(bool, WillSuicide) { _willSuicide = value; }
    PROPERTY(WillSuicide)

    GETTER(bool, StartAnimationOnPlay) { return ReflectFields->StartAnimationOnPlay; }
    SETTER(bool, StartAnimationOnPlay) { ReflectFields->StartAnimationOnPlay = value; }
    PROPERTY(StartAnimationOnPlay)

public:
    void Setup();
    void StartAnimation();
    void StopAnimation();
    float GetAnimationProgress(int targetFrame) const;

protected:
    void Start() override;
    void Update() override;

    void ResetUV();

    void UpdateFrame();

    void EditorUpdate();

    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;

    void ImGuiDrawPropertysEvent() override;

protected:
    REFLECT_FIELDS_BEGIN(ImageElement)
    bool  Loop                 = false;
    float Duration             = 1.0f;
    int   EmptyFrameCount      = 0;
    bool  StartAnimationOnPlay = false;
    REFLECT_FIELDS_END(SpriteAnimationElement)

    float _elapsedTime = 0.0f;
    float _durationPerFrame = 0.0f;
    int   _currentFrame     = 0;
    bool  _isPlaying        = false;
    bool  _willSuicide      = false;
};
