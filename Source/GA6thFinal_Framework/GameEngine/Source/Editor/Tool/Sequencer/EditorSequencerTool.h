#pragma once
#include "Engine/EngineCore/TimelineSystem.h"

class EditorSequencerTool : public EditorTool
{
public:
    EditorSequencerTool();
    virtual ~EditorSequencerTool();

public:
    virtual void OnTickGui() override;
    virtual void OnStartGui() override;
    virtual void OnEndGui() override;

    virtual void OnPreFrameBegin();
    virtual void OnPostFrameBegin();
    virtual void OnFrameRender();
    virtual void OnFrameEnd();
    virtual void OnFrameFocusEnter();
    virtual void OnFrameFocusStay();
    virtual void OnFrameFocusExit();
    virtual void OnFramePopupOpened();

private:
    TimelineSystem* _timelineSystem = nullptr;
};

class TestTimeLineEvent : public ITimelineEvent
{
    USING_PROPERTY(TestTimeLineEvent)
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
    REFLECT_FIELDS_END(TestTimeLineEvent)
};