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
