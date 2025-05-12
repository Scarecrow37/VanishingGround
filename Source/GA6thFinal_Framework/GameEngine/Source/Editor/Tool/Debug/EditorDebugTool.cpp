#include "pch.h"
#include "EditorDebugTool.h"
using namespace Global;

EditorDebugTool::EditorDebugTool() 
    : 
    _frameCount(),
    _elpasedTime(),
    _fps()
{
    SetLabel("Debug");
    SetDockLayout(DockLayout::DOWN);
}

EditorDebugTool::~EditorDebugTool()
{
}

void EditorDebugTool::OnTickGui()
{
}

void EditorDebugTool::OnStartGui()
{
    return void ();
}

void EditorDebugTool::OnPreFrame()
{
    return void ();
}

void EditorDebugTool::OnFrame()
{
    ImGui::InputDouble("Time scale", &engineCore->Time.TimeScale);

    ImGui::Text("Time : %f", engineCore->Time.Time());
    ImGui::Text("RealtimeSinceStartup : %f", engineCore->Time.RealtimeSinceStartup());

    ImGui::Text("FrameCount : %llu", engineCore->Time.FrameCount());

    UpdateFPS();
    ImGui::Text("FPS : %d", _fps);

    ImGui::Text("DeltaTime : %f", engineCore->Time.DeltaTime());

    ImGui::Text("UnscaledDeltaTime : %f", engineCore->Time.UnscaledDeltaTime());

    ImGui::InputDouble("Fixed Time Step", &engineCore->Time.FixedTimeStep);
    ImGui::Text("FixedDeltaTime %f", engineCore->Time.FixedDeltaTime());
    ImGui::Text("FixedUnscaledDeltaTime %f", engineCore->Time.FixedUnscaledDeltaTime());

    ImGui::InputDouble("maximumDeltaTime", &engineCore->Time.MaximumDeltaTime);
}

void EditorDebugTool::OnPostFrame()
{
    return void ();
}

void EditorDebugTool::OnPopup() 
{

}

void EditorDebugTool::UpdateFPS() 
{
    ++_frameCount;
    _elpasedTime += UmTime.DeltaTime();
    if (_elpasedTime >= 0.5f)
    {
        _fps         = static_cast<int>((float)_frameCount / _elpasedTime);
        _frameCount  = 0;
        _elpasedTime = 0.0;
    }
}
