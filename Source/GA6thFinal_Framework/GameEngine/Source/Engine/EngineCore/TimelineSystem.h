#pragma once

struct TimelineNotify
{
    using Event = std::function<void()>;

    std::string Name;
    float Time;
    Event NotifyEvent;
};

class TimelineSystem : public ReflectSerializer
{
public:
    TimelineSystem();
    ~TimelineSystem();

public:
    void Update();

    void SetActive(bool isActive);

    void SetLoop(bool isLoop);

    void SetNotifyEnabled(bool isEnabled);

    void ClearNotifies();

    void ResetFrame();

    void SetMaxFrame(float maxFrame);

    void SetCurrentFrame(float frame);

    void AddNotify(const char* name, float time, const TimelineNotify::Event& event);

    bool RemoveNotify(const char* name);

    /* Getter */
    inline float GetMaxFrame() const { return _maxFrame; }
    inline float GetCurrentFrame() const { return _currFrame; }
    inline float GetPreviousFrame() const { return _prevFrame; }
    inline bool  IsActive() const { return _isActive; }
    inline bool  IsLoop() const { return _isLoop; }
    inline bool  IsNotifyEnabled() const { return _isNotifyEnabled; }

private:
    void ProcessNotifies(float startTime, float endTime);

private:
    float _maxFrame;
    float _currFrame;
    float _prevFrame;

    bool _isActive;
    bool _isLoop;
    bool _isNotifyEnabled;
    std::vector<TimelineNotify> _timelineNotifyQueue;
    std::unordered_map<std::string, TimelineNotify*> _timelineNotifyTable;
};