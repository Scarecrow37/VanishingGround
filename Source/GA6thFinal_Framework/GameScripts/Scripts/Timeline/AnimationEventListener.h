#pragma once
#include "UmFramework.h"

class SkeletalMeshRenderer;

class AnimationEventListener : public Component
{
    USING_PROPERTY(AnimationEventListener)

public:
    AnimationEventListener();
    virtual ~AnimationEventListener();

public:
    REFLECT_PROPERTY(FilePath)
    GETTER_ONLY(std::string, FilePath) { return _filePath.string(); }
    PROPERTY(FilePath)

public:
    void Start() override;
    void Update() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void ImGuiDrawPropertysEvent() override;

public:
    bool IsLoadedAnimationNotifySet() const;
    bool IsLoadedSkeletalMeshRenderer() const;
    void SetAnimationNotifyFromGuid(const File::Guid& guid);
    void SetAnimationNotifyFromPath(const File::Path& path);

protected:
    File::GuidRef         _guidRef;
    File::Path            _filePath;
    AnimationNotifySet    _animationNotifySet;
    SkeletalMeshRenderer* _skeletalMeshRenderer = nullptr;

    REFLECT_FIELDS_BEGIN(Component)
    std::string Guid;
    REFLECT_FIELDS_END(AnimationEventListener)
};

class TestTimeLineEvent_1 : public ITimelineEvent
{
    USING_PROPERTY(TestTimeLineEvent_1)
private:
    // ITimelineEvent을(를) 통해 상속됨
    void OnNotified(float time) override;

public:
    REFLECT_PROPERTY(Time)
    GETTER(float, Time) { return _time; }
    SETTER(float, Time) { _time = value; }
    PROPERTY(Time)

private:
    float _time;
    REFLECT_FIELDS_BEGIN(ITimelineEvent)
    REFLECT_FIELDS_END(TestTimeLineEvent_1)
};

class TestTimeLineEvent_2 : public ITimelineEvent
{
    USING_PROPERTY(TestTimeLineEvent_2)
private:
    // ITimelineEvent을(를) 통해 상속됨
    void OnNotified(float time) override;

public:
    REFLECT_PROPERTY(Time, Dummy)
    GETTER(float, Time) { return _time; }
    SETTER(float, Time) { _time = value; }
    PROPERTY(Time)
    GETTER(float, Dummy) { return _dummy; }
    SETTER(float, Dummy) { _dummy = value; }
    PROPERTY(Dummy)

private:
    float _time;
    float _dummy;
    REFLECT_FIELDS_BEGIN(ITimelineEvent)
    REFLECT_FIELDS_END(TestTimeLineEvent_2)
};