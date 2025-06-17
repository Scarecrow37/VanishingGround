#include "pch.h"
#include "EditorSequencer.h"

EditorSequencer::EditorSequencer() 
{
}

EditorSequencer::~EditorSequencer() 
{
}

void EditorSequencer::Render(TimelineSystem* system) 
{
    if (nullptr == system)
    {
        return;
    }
    ImGui::PushID(system);
    {
        ImGui::Text("Sequencer");
        ImGui::Separator();
        bool isPlaying = system->IsPlaying();
        if (ImGui::Checkbox("Play", &isPlaying))
        {
            isPlaying ? system->Play() : system->Stop();
        }
        bool isLoop = system->HasFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_LOOP);
        if (ImGui::Checkbox("Loop", &isLoop))
        {
            system->ToggleFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_LOOP);
        }
        bool isNotifyDisabled = system->HasFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_NOTIFY_DISABLED);
        if (ImGui::Checkbox("Notify Disabled", &isNotifyDisabled))
        {
            system->ToggleFlags(TimelineSystem::TIMELINESYSTEM_FLAGS_NOTIFY_DISABLED);
        }
        float maxFrame = system->GetMaxFrame();
        ImGui::SliderFloat("Max Frame", &maxFrame, 0.0f, 100.0f);
        system->SetMaxFrame(maxFrame);
        float currentFrame = system->GetCurrentFrame();
        ImGui::SliderFloat("Current Frame", &currentFrame, 0.0f, maxFrame);
        system->SetCurrentFrame(currentFrame);
        if (ImGui::Button("Clear Notifies"))
        {
            system->ClearNotifies();
        }
    }
    ImGui::PopID();
}
