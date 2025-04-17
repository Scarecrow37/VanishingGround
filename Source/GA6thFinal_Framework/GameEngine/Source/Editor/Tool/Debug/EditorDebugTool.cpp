#include "pch.h"
#include "EditorDebugTool.h"
using namespace Global;

EditorDebugTool::EditorDebugTool()
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
    ImGui::InputDouble("time scale", &engineCore->Time.TimeScale);

    ImGui::Text("time : %f", engineCore->Time.time());
    ImGui::Text("realtimeSinceStartup : %f", engineCore->Time.realtimeSinceStartup());

    ImGui::Text("frameCount : %llu", engineCore->Time.frameCount());

    ImGui::Text("FPS : %d", engineCore->Time.frameRate());
    ImGui::Text("DeltaTime : %f", engineCore->Time.deltaTime());

    ImGui::Text("unscaledDeltaTime : %f", engineCore->Time.unscaledDeltaTime());

    ImGui::InputDouble("Fixed Time Step", &engineCore->Time.FixedTimeStep);
    ImGui::Text("fixedDeltaTime %f", engineCore->Time.fixedDeltaTime());
    ImGui::Text("fixedUnscaledDeltaTime %f", engineCore->Time.fixedUnscaledDeltaTime());

    ImGui::InputDouble("maximumDeltaTime", &engineCore->Time.MaximumDeltaTime);
}

void EditorDebugTool::OnPostFrame()
{
    return void ();
}

void EditorDebugTool::OnPopup() 
{
}
