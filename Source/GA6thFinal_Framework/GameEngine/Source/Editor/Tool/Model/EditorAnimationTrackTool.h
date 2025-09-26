#pragma once

class EditorAnimationTrackTool : public EditorTool
{
public:
    EditorAnimationTrackTool();
    virtual ~EditorAnimationTrackTool();

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

private:
    void UpdateTimeline();

    void DrawMenuBar();
    void DrawTimelines();
    void DrawCanvas();
    void DrawDetails();

    bool NewFileWithDialog();
    bool LoadFileWithDialog();
    bool SaveFileWithDialog();

    void SetEventTrackFromAnimation(std::string_view animKey);
    void AddEventTrackFromAnimation(std::string_view animKey);
    void RemoveEventTrackFromAnimation(std::string_view animKey);
    void AddEvent(std::string_view label, std::string_view animKey, std::string_view typeNameID, float time = FLT_MAX);
    
    bool ShowEventTrackList(std::shared_ptr<Timeline::EventTrack> track);
    void ShowEventTrackEditTab(std::shared_ptr<Timeline::EventTrack> track, UINT contextID);
    void ShowAvailableEventTracks();

    void FocusDetailTabItem(int index);

    void LowerFramePopup(Timeline::EventTrack& track);
    void ContextPopup(Timeline::EventTrack& track, Timeline::EventContext& context);

    const std::string& GetCurrentDetailAnimName();
    const std::string& GetCurrentEventTrackmName();

    bool IsLoadAsset() const;

private:
    EditorModelDetails* _modelDetails = nullptr;
    Timeline::SequencerEditor* _sequencer = nullptr;
    AnimationEventTrack _animationEventTrack;
    std::queue<std::function<void()>> _eventQueue;

    std::string _copyBuffer;

    // DetailFrame
    ImGuiTabBar*  _tabbar     = nullptr;
    ImGuiTabItem* _tabItem[3] = {nullptr, nullptr, nullptr};

    inline static constexpr const char* TAB_LABEL[] = {"List", "Edit", "Debug"};

};