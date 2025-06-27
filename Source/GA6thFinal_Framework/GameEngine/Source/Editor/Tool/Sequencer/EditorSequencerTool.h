#pragma once

class EditorModelDetails;
class Animator;

class EditorSequencerTool : public EditorTool
{
public:
    EditorSequencerTool();
    virtual ~EditorSequencerTool();

public:
    void OnTickGui() override;
    void OnStartGui() override;
    void OnEndGui() override;

    void OnPreFrameBegin() override;
    void OnPostFrameBegin() override;
    void OnFrameRender() override;
    void OnFrameEnd() override;
    void OnFrameFocusEnter() override;
    void OnFrameFocusStay() override;
    void OnFrameFocusExit() override;
    void OnFramePopupOpened() override;

    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    void ShowTimelineNotifies();

private:
    std::shared_ptr<TimelineSystem> _timelineSystem;
    EditorSequencer* _sequencer     = nullptr;

    REFLECT_FIELDS_BEGIN(EditorTool)
    std::string SerializedData;
    REFLECT_FIELDS_END(EditorSequencerTool)
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
    REFLECT_PROPERTY(Time)
    GETTER(float, Time) { return _time; }
    SETTER(float, Time) { _time = value; }
    PROPERTY(Time)

private:
    float _time;
    REFLECT_FIELDS_BEGIN(ITimelineEvent)
    REFLECT_FIELDS_END(TestTimeLineEvent_2)
};