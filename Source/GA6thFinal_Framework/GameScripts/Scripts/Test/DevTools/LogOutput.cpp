#include "pchScripts.h"
#include "LogOutput.h"
LogOutput::LogOutput()
{
    ReflectFields->LogColorTable[LogLevel::LEVEL_TRACE]   = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_TRACE);
    ReflectFields->LogColorTable[LogLevel::LEVEL_DEBUG]   = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_DEBUG);
    ReflectFields->LogColorTable[LogLevel::LEVEL_INFO]    = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_INFO);
    ReflectFields->LogColorTable[LogLevel::LEVEL_WARNING] = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_WARNING);
    ReflectFields->LogColorTable[LogLevel::LEVEL_ERROR]   = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_ERROR);
    ReflectFields->LogColorTable[LogLevel::LEVEL_FATAL]   = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_FATAL);
}
LogOutput::~LogOutput() = default;

void LogOutput::Update() 
{
#ifndef _UMEDITOR
    ShowLog();
#endif
}

void LogOutput::ShowLog() 
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

    ImGui::Begin("logger##3050EF43-0474-4D39-86CD-D0EBCADFC655", nullptr, flags);
    {
        auto& logs = UmLogger.GetLogMessages();
        static ImGuiListClipper clipper;
        clipper.Begin((int)logs.size());
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                static std::string logText;
                logText.clear();
                auto& [level, message, location] = logs[i];
                logText += message;
                bool isLog = false == UmLogger.IsMessageLocation(location);
                if (true == isLog)
                {
                    logText += std::format(" {}, line : {}", location.function_name(), location.line());
                }
                ImGui::PushID(&message);
                ImGui::PushStyleColor(ImGuiCol_Text, ImGuiHelper::ArrayToImVec4(ReflectFields->LogColorTable[level]));
                ImGui::Text(logText.c_str());
                ImGui::PopStyleColor();
                ImGui::PopID();
            }
        }
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::End();
}
