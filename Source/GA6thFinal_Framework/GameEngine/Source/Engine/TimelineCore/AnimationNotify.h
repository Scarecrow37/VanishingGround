#pragma once

class AnimationNotify : public ReflectSerializer
{
public:
    AnimationNotify();
    ~AnimationNotify();

public:


private:
    File::Guid _modelGuid;
    std::shared_ptr<TimelineSystem> _timelineSystem;
    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(AnimationNotify)
};
