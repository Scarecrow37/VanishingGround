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
    _timelineSystem->SetActive(true);
    _timelineSystem->SetMaxFrame(5.0f);
    _timelineSystem->AddNotify("TestNotify1", 1.0f, []() { UmLogger.Log(1, "Timeline TestNotify1 Triggered at 1.0s"); });
    _timelineSystem->AddNotify("TestNotify2", 3.0f, []() { UmLogger.Log(1, "Timeline TestNotify2 Triggered at 3.0s"); });
    _timelineSystem->AddNotify("TestNotify3", 3.0f, []() { UmLogger.Log(1, "Timeline TestNotify3 Triggered at 3.0s"); });
}

void EditorSequencerTool::OnEndGui() {}

void EditorSequencerTool::OnPreFrameBegin() {}

void EditorSequencerTool::OnPostFrameBegin() {}

void EditorSequencerTool::OnFrameRender() 
{
    float maxFrame = _timelineSystem->GetMaxFrame();
    float currentFrame = _timelineSystem->GetCurrentFrame();
    ImGui::SliderFloat("Current Frame", &currentFrame, 0.0f, maxFrame);

    static bool isActive = _timelineSystem->IsActive();
    if (ImGui::Checkbox("Active", &isActive))
    {
        _timelineSystem->SetActive(isActive);
    }
    static bool isLoop = _timelineSystem->IsLoop();
    if (ImGui::Checkbox("Loop", &isLoop))
    {
        _timelineSystem->SetLoop(isLoop);
    }
}

void EditorSequencerTool::OnFrameEnd() {}

void EditorSequencerTool::OnFrameFocusEnter() {}

void EditorSequencerTool::OnFrameFocusStay() {}

void EditorSequencerTool::OnFrameFocusExit() {}

void EditorSequencerTool::OnFramePopupOpened() {}
