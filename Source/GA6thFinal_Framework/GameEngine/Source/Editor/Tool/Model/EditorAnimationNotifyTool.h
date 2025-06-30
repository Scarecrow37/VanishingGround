#pragma once

class EditorAnimationNotifyTool : public EditorTool
{
public:
    EditorAnimationNotifyTool();
    virtual ~EditorAnimationNotifyTool();

private:
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

public:
    void GetModelDetailsToolInDock();

private:
    void UpdateTimeline();

    void DrawMenuBar();
    void DrawTimelines();
    void DrawCanvas();
    void DrawDetails();

private:
    EditorModelDetails* _modelDetails = nullptr;
    EditorSequencer* _sequencer = nullptr;

    AnimationNotifySet _animationNotifySet;
};
