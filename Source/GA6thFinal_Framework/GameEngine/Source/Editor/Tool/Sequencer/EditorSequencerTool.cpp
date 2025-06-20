#include "pch.h"
#include "EditorSequencerTool.h"

EditorSequencerTool::EditorSequencerTool() 
{
    SetLabel("Sequencer");
    _timelineSystem = std::make_shared<TimelineSystem>();
    _sequencer      = new EditorSequencer();
    _sequencer->SetSystem(_timelineSystem);
}

EditorSequencerTool::~EditorSequencerTool() 
{
    if (_sequencer)
    {
        delete _sequencer;
        _sequencer = nullptr;
    }
}
static float max         = 5.5f;
static float min         = -1.5f;
static float elapsedTime = 0.0f;
static float scale       = 1.0f;

void EditorSequencerTool::OnTickGui() 
{
    //elapsedTime += UmTime.DeltaTime() * scale;
    //if (elapsedTime > max)
    //{
    //    elapsedTime += min - max;
    //}
    //_timelineSystem->SetCurrentFrame(elapsedTime);
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
    static bool firstTick = true;
    if (firstTick)
    {
        firstTick = false;
        _timelineSystem->Play();
        _timelineSystem->SetMinFrame(min);
        _timelineSystem->SetMaxFrame(max);
        //_timelineSystem->AddNotify<TestTimeLineEvent>(min)->Time  = min;
        //_timelineSystem->AddNotify<TestTimeLineEvent>(1.0f)->Time = 1.0f;
        //_timelineSystem->AddNotify<TestTimeLineEvent>(3.0f)->Time = 3.0f;
        //_timelineSystem->AddNotify<TestTimeLineEvent>(3.0f)->Time = 3.0f;
        //_timelineSystem->AddNotify<TestTimeLineEvent>(1.5f)->Time = 1.5f;
        //_timelineSystem->AddNotify<TestTimeLineEvent>(max)->Time  = max;
        //_timelineSystem->AddNotify<TestTimeLineEvent>(0.0f)->Time = 0.0f;
    }

    float maxFrame     = _timelineSystem->GetMaxFrame();
    float minFrame     = _timelineSystem->GetMinFrame();
    float currentFrame = _timelineSystem->GetCurrentFrame();

    if (ImGui::SliderFloat("Scale", &scale, 0.0f, 10.0f))
    {

    }

    if (ImGui::SliderFloat("Current Frame", &currentFrame, minFrame, maxFrame))
    {
        _timelineSystem->SetCurrentFrame(currentFrame, false);
    }

    if (ImGui::DragFloat("Max Frame", &maxFrame, 0.1f, -FLT_MAX, FLT_MAX))
    {
        _timelineSystem->SetMaxFrame(maxFrame);
    }
    if (ImGui::DragFloat("Min Frame", &minFrame, 0.1f, -FLT_MAX, FLT_MAX))
    {
        _timelineSystem->SetMinFrame(minFrame);
    }

    bool isActive = _timelineSystem->IsActive();
    if (ImGui::Checkbox("Active", &isActive))
    {
        _timelineSystem->SetActive(isActive);
    }
    bool isPlaying = _timelineSystem->IsPlaying();
    if (ImGui::Checkbox("Playing", &isPlaying))
    {
        isPlaying ? _timelineSystem->Play() : _timelineSystem->Stop();
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


    //snap
    ImGui::Checkbox("Use Snapping", &_sequencer->_useSnapping);

    ImGui::BeginChild("SequencerCanvas", ImVec2(0, 0), true);
    _sequencer->Render();
    ImGui::EndChild();
}

void EditorSequencerTool::OnFrameEnd() {}

void EditorSequencerTool::OnFrameFocusEnter() {}

void EditorSequencerTool::OnFrameFocusStay() {}

void EditorSequencerTool::OnFrameFocusExit() {}

void EditorSequencerTool::OnFramePopupOpened() {}

void EditorSequencerTool::SerializedReflectEvent() 
{
    ReflectFields->SerializedData = _timelineSystem->SerializedReflectFields();
}

void EditorSequencerTool::DeserializedReflectEvent() 
{
    _timelineSystem->DeserializedReflectFields(ReflectFields->SerializedData);
}

REGISTER_CLASS(TimelineSystem, TestTimeLineEvent)
void TestTimeLineEvent::OnNotified(float time)
{
    std::string message = "Timeline TestNotify Triggered at " + std::to_string(time) + "s";
    UmLogger.Log(1, message);
}
