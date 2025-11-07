#include "pchScripts.h"
#include "LogOutput.h"


UMREAL_COMPONENT(LogOutput)

LogOutput::LogOutput()
{
    _isShowLog     = false;

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
    Debugger dbg;
    dbg([this]() 
    {
        DrawShowLog();
        DrawLogSetting();
    }); 
}

void LogOutput::DrawShowLog() 
{
    if (_isShowLog)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs;

        ImGui::Begin("logger##3050EF43-0474-4D39-86CD-D0EBCADFC655", &_isShowLog, flags);
        {
            auto&                   logs = UmLogger.GetLogMessages();
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
                    ImGui::PushStyleColor(ImGuiCol_Text,
                                          ImGuiHelper::ArrayToImVec4(ReflectFields->LogColorTable[level]));
                    ImGui::Text(logText.c_str());
                    ImGui::PopStyleColor();
                    ImGui::PopID();
                }
            }
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::End();
    }
}

void LogOutput::DrawLogSetting() 
{
    if (ImGui::TreeNode("Log Output"))
    {
        ImGui::Checkbox("Show log", &_isShowLog);
        ImGui::Separator();
        ImGui::Text("Font size");
        float& globalScale = ImGui::GetIO().FontGlobalScale;
        if (ImGui::DragFloat("##font scale", &globalScale, 0.01f))
        {
            globalScale = std::clamp(globalScale, 0.1f, 10.f);
        }
        ImGui::TreePop();
    }
}
