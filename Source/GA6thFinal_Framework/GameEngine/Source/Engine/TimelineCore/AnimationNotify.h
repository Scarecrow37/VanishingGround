#pragma once

class AnimationNotify
{
public:
    AnimationNotify();
    ~AnimationNotify();

public:


private:
    File::Guid _modelGuid;
    std::shared_ptr<TimelineSystem> _timelineSystem;
};
