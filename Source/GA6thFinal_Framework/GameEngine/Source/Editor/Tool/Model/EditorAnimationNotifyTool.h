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

    void AddTimelineFromAnimation(std::string_view animKey);
    void AddNotify(std::string_view animKey, std::string_view typeNameID, float time = FLT_MAX);
    void RemoveTimeline(std::string_view animKey);

    bool ShowNotifyList(std::shared_ptr<TimelineSystem> system);
    void ShowNotifyEditTab(std::shared_ptr<TimelineSystem> system, UINT notifyID);

private:
    EditorModelDetails* _modelDetails = nullptr;
    EditorSequencer* _sequencer = nullptr;
    AnimationNotifySet _animationNotifySet;
    std::queue<std::function<void()>> _eventQueue;

    // DetailFrame
    std::string _tabLabel[2]    = {"List", "Edit"};
};
