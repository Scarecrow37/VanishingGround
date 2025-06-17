#include "pch.h"
#include "EditorSequencerTool.h"

EditorSequencerTool::EditorSequencerTool() 
{
    SetLabel("Sequencer");
    _timelineSystem = new TimelineSystem;
}

EditorSequencerTool::~EditorSequencerTool() {}

void EditorSequencerTool::OnTickGui() 
{
    _timelineSystem->Update();
}

void EditorSequencerTool::OnStartGui() 
{
}

void EditorSequencerTool::OnEndGui() {}

void EditorSequencerTool::OnPreFrameBegin() {}

void EditorSequencerTool::OnPostFrameBegin() {}

void EditorSequencerTool::OnFrameRender() 
{
    static bool firstTick    = true;
    if (firstTick)
    {
        firstTick = false;
        float max = 5.0f;
        _timelineSystem->Play();
        _timelineSystem->SetMaxFrame(max);
        _timelineSystem->AddNotify<TestTimeLineEvent>(1.0f)->Time = 1.0f;
        _timelineSystem->AddNotify<TestTimeLineEvent>(3.0f)->Time = 3.0f;
        _timelineSystem->AddNotify<TestTimeLineEvent>(3.0f)->Time = 3.0f;
        _timelineSystem->AddNotify<TestTimeLineEvent>(1.5f)->Time = 1.5f;
        _timelineSystem->AddNotify<TestTimeLineEvent>(max)->Time  = max;
        _timelineSystem->AddNotify<TestTimeLineEvent>(0.0f)->Time = 0.0f;
    }

    float maxFrame = _timelineSystem->GetMaxFrame();
    float currentFrame = _timelineSystem->GetCurrentFrame();
    ImGui::SliderFloat("Current Frame", &currentFrame, 0.0f, maxFrame);

    bool isActive = _timelineSystem->IsPlaying();
    if (ImGui::Checkbox("Active", &isActive))
    {
        isActive ? _timelineSystem->Play() : _timelineSystem->Stop();
    }
    bool isLoop = _timelineSystem->HasFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_LOOP);
    if (ImGui::Checkbox("Loop", &isLoop))
    {
        _timelineSystem->ToggleFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_LOOP);
    }
    bool isNotifyDisable = _timelineSystem->HasFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_NOTIFY_DISABLED);
    if (ImGui::Checkbox("Notify Disable", &isNotifyDisable))
    {
        _timelineSystem->ToggleFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_NOTIFY_DISABLED);
    }
    bool isCounter = _timelineSystem->HasFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_USE_COUNTER);
    if (ImGui::Checkbox("Use Counter", &isCounter))
    {
        _timelineSystem->ToggleFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_USE_COUNTER);
    }

    auto list = _timelineSystem->GetTimelineNotifyList();
    for (const auto& notify : list)
    {
        ImGui::Separator();
        notify->GetEvent()->ImGuiDrawPropertys();
    }
}

void EditorSequencerTool::OnFrameEnd() {}

void EditorSequencerTool::OnFrameFocusEnter() {}

void EditorSequencerTool::OnFrameFocusStay() {}

void EditorSequencerTool::OnFrameFocusExit() {}

void EditorSequencerTool::OnFramePopupOpened() {}

REGISTER_CLASS(TimelineSystem, TestTimeLineEvent)
void TestTimeLineEvent::OnNotified(float time)
{
    std::string message = "Timeline TestNotify Triggered at " + std::to_string(time) + "s";
    UmLogger.Log(1, message);
}
