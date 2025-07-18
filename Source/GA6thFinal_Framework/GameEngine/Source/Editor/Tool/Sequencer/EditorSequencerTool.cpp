#include "pch.h"
#include "EditorSequencerTool.h"

EditorSequencerTool::EditorSequencerTool() 
{
    SetLabel("Sequencer");
    SetDockLayout(ImGuiDir_Down);
    _timelineSystem = std::make_shared<TimelineSystem>();
    _sequencer      = new EditorSequencer();
    _sequencer->SetSystem(_timelineSystem);
    
    UINT flags = EditorSequencer::FLAGS_DEBUG | 
                 EditorSequencer::FLAGS_USE_SNAP |
                 EditorSequencer::FLAGS_USE_DRAG_MIN_MAX_FRAME | 
                 EditorSequencer::FLAGS_USE_DRAG_FRAME_LINE;
    _sequencer->AddFlags(flags);
}

EditorSequencerTool::~EditorSequencerTool() 
{
    if (_sequencer)
    {
        delete _sequencer;
        _sequencer = nullptr;
    }
}

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
        _timelineSystem->SetMinFrame(0.0f);
        _timelineSystem->SetMaxFrame(5.0f);
    }

    float maxFrame     = _timelineSystem->GetMaxFrame();
    float minFrame     = _timelineSystem->GetMinFrame();
    float currentFrame = _timelineSystem->GetCurrentFrame();

    bool useSnapping = _sequencer->HasFlags(EditorSequencer::FLAGS_USE_SNAP);
    if (ImGui::Checkbox("Use Snapping", &useSnapping))
    {
        _sequencer->ToggleFlags(EditorSequencer::FLAGS_USE_SNAP);
    }

    _timelineSystem->Update();

    ImVec2 availSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("SequencerCanvas", ImVec2(availSize.x - 400, availSize.y), true,
                      ImGuiWindowFlags_NoScrollWithMouse);
    _sequencer->Show();
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
