#include "pch.h"
#include "EditorSequencerTool.h"

#include "Engine/GraphicsCore/Animator.h"

EditorSequencerTool::EditorSequencerTool() 
{
    SetLabel("Sequencer");
    SetDockLayout(ImGuiDir_Down);
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
}

void EditorSequencerTool::OnStartGui() 
{
    auto& system    = Global::editorModule->GetDockWindowSystem();
    auto* modelDock = system.GetDockWindow("ModelDock");
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
    }

    float maxFrame     = _timelineSystem->GetMaxFrame();
    float minFrame     = _timelineSystem->GetMinFrame();
    float currentFrame = _timelineSystem->GetCurrentFrame();
    ImGui::Checkbox("Use Snapping", &_sequencer->_useSnapping);

    _timelineSystem->Update();

    ImVec2 availSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("SequencerCanvas", ImVec2(availSize.x - 400, availSize.y), true,
                      ImGuiWindowFlags_NoScrollWithMouse);
    _sequencer->Show(true);
    ImGui::EndChild();
    ImGui::SameLine();
    availSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("SequencerDetail", ImVec2(availSize), true);
    if (ImGui::CollapsingHeader("Sequencer Debug"))
    {
        _sequencer->ShowDebugData();
    }
    if (ImGui::CollapsingHeader("Timeline Notifies"))
    {
        ShowTimelineNotifies();
    }
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

void EditorSequencerTool::ShowTimelineNotifies() 
{
    for (const auto& notify : _timelineSystem->GetTimelineNotifyList())
    {
        ImGui::PushID(notify);

        char             buf[64];
        std::string_view label = notify->Label;
        float            time  = notify->Time;
        UINT             id    = notify->ID;
        strcpy_s(buf, label.data()); // copy the label to a buffer
        ImGui::Text("Label: ");
        ImGui::SameLine();
        if (ImGui::InputText("Label", buf, sizeof(buf)))
        {
            notify->Label = buf; // update the label in the notify
        }
        ImGui::Text("Time: %.2f", time);
        ImGui::Text("ID: %d", id);
        if (ImGui::TreeNodeEx("Event", ImGuiTreeNodeFlags_DefaultOpen))
        {
            std::string_view eventName = notify->EventName;
            ImGui::Text("Event Name: %s", eventName.data() + 6);
            notify->Event->ImGuiDrawPropertys();
            ImGui::TreePop();
        }
        ImGui::Separator();

        ImGui::PopID();
    }
}

REGISTER_CLASS(TimelineSystem, TestTimeLineEvent_1)
void TestTimeLineEvent_1::OnNotified(float time)
{
    std::string message = "TestTimeLineEvent_1 Notify at " + std::to_string(time) + "s";
    UmLogger.Log(2, message);
}
REGISTER_CLASS(TimelineSystem, TestTimeLineEvent_2)
void TestTimeLineEvent_2::OnNotified(float time) 
{
    std::string message = "TestTimeLineEvent_2 Notify at " + std::to_string(time) + "s";
    UmLogger.Log(2, message);
}
