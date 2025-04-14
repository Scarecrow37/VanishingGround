#include "EditorDebugView.h"
using namespace Global;

EditorDebugView::EditorDebugView()
{
    SetLabel("Debug");
    SetInitialDockSpaceArea(DockSpaceArea::DOWN);
}

EditorDebugView::~EditorDebugView()
{
}

void EditorDebugView::OnTickGui()
{
}

void EditorDebugView::OnStartGui()
{
    return void ();
}

void EditorDebugView::OnPreFrame()
{
    return void ();
}

void EditorDebugView::OnFrame()
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

void EditorDebugView::OnPostFrame()
{
    return void ();
}