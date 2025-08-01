#include "pchScripts.h"
#include "FPSCounter.h"
FPSCounter::FPSCounter()
{
    _frameCount = 0;
    _elapsedTime = 0.f;
    _fps         = 0;
    _showFPS     = false;
}
FPSCounter::~FPSCounter() = default;

void FPSCounter::Update()
{
    if (ImGui::IsKeyReleased(ImGuiKey_F2))
    {
        ToggleFPS();
    }
    UpdateFPS();
    ShowFPS();
}

void FPSCounter::UpdateFPS()
{
    ++_frameCount;
    _elapsedTime += UmTime.UnscaledDeltaTime();
    if (_elapsedTime >= 0.5f)
    {
        _fps         = static_cast<int>((float)_frameCount / _elapsedTime);
        _frameCount  = 0;
        _elapsedTime = 0.0;
    }
}

void FPSCounter::ShowFPS() 
{
    if (_showFPS)
    {
        ImGui::Begin("FPS Counter##96CF026F-135B-4B8E-B41E-54F418784D00", &_showFPS);
        ETimeSystem& time = UmTime;
        ImGui::InputDouble("Time scale", &time.TimeScale);

        ImGui::Text("Time : %f", UmTime.Time());
        ImGui::Text("RealtimeSinceStartup : %f", time.RealtimeSinceStartup());

        ImGui::Text("FrameCount : %llu", time.FrameCount());

        UpdateFPS();
        ImGui::Text("FPS : %d", _fps);

        ImGui::Text("DeltaTime : %f", time.DeltaTime());

        ImGui::Text("UnscaledDeltaTime : %f", time.UnscaledDeltaTime());

        ImGui::InputDouble("Fixed Time Step", &time.FixedTimeStep);
        ImGui::Text("FixedDeltaTime %f", time.FixedDeltaTime());
        ImGui::Text("FixedUnscaledDeltaTime %f", time.FixedUnscaledDeltaTime());

        ImGui::InputDouble("maximumDeltaTime", &time.MaximumDeltaTime);

        ImGui::Separator();
        ImGui::Text("Scene");
        if (ImGui::Button("Reload Scene"))
        {
            const std::string& name = UmSceneManager.GetMainScene()->Name;
            UmSceneManager.LoadScene(name);
        }

        ImGui::Separator();
        ImGui::Text("Font size");
        float& globalScale = ImGui::GetIO().FontGlobalScale;
        if (ImGui::DragFloat("##font scale", &globalScale, 0.01f))
        {
            globalScale = std::clamp(globalScale, 0.1f, 10.f);
        }

        ImGui::End();
    }
}
