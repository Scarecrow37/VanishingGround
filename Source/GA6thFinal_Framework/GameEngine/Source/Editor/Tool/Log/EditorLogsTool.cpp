#include "pch.h"
#include "EditorLogsTool.h"

using namespace u8_literals;
using namespace Global;

EditorLogsTool::EditorLogsTool()
{
    SetLabel(u8"로그###로그"_c_str);
    SetDockLayout(DockLayout::DOWN);
    ResetLogColor();
    ResetLogFilter();
}

EditorLogsTool::~EditorLogsTool()
{
}

void EditorLogsTool::OnStartGui()
{
}

void EditorLogsTool::OnEndGui() 
{
}

static bool openVSWithFile(const std::string& filePath, int lineNumber)
{
    std::string processPath; 
    INT_PTR result{};
    while (true)
    {
        ProcessHelper::IsVisualStudio(processPath);
        if (processPath.empty() == false)
        {
            result = (INT_PTR) ShellExecuteA(NULL, "open", filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
            return true;
        }
        if (result <= 32)
        {
            constexpr const wchar_t* ScriptSlnFilePath = L"..\\GA6thFinal_Framework.sln";
            result = (INT_PTR)ShellExecuteW(NULL, L"open", ScriptSlnFilePath, NULL, NULL, SW_SHOWNORMAL);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void EditorLogsTool::OnTickGui() 
{
    const auto& logMessages = engineCore->Logger.GetLogMessages();
    if (prevLogCount < logMessages.size())
    {
        for (size_t i = prevLogCount; i < logMessages.size(); i++)
        {
            auto& [level, message, location] = logMessages[i];
            if (ReflectFields->LogFilterTable[level] == true)
            {
                _drawLogList.emplace_back(level, message, location);   
            }          

            if (_isFocused == false)
            {
                notReadCount++;
            }
        }
        prevLogCount = logMessages.size();
        _isMessagePush = true;
        if (IsVisible() == false)
        {
            SetVisible(true);
        }
    }
}

void EditorLogsTool::OnPreFrame() 
{
    std::string lable = u8"로그###로그"_c_str;
    if (notReadCount > 0)
    {
        static float    elapsedTime   = 0.f;
        static bool     reverse       = false;
        constexpr float animationTime = 1.f;
        elapsedTime += UmTime.UnscaledDeltaTime();
        float t = elapsedTime / animationTime;
        if (elapsedTime >= animationTime)
        {
            elapsedTime = 0.f;
            reverse     = !reverse;
        }
        ImVec4&          defaultTextColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
        auto& [level, message, location]  = _drawLogList.back();
        ImVec4 logTextColor = ImGuiHelper::ArrayToImVec4(ReflectFields->LogColorTable[level]);
        ImVec4 lerpColor = reverse ? ImGuiHelper::ImVec4Lerp(logTextColor, defaultTextColor, t)
                                   : ImGuiHelper::ImVec4Lerp(defaultTextColor, logTextColor, t);
        ImGui::PushStyleColor(ImGuiCol_Text, lerpColor);
        lable = std::format("{} +{}###{}", u8"로그"_c_str, std::min(notReadCount, (size_t)999), u8"로그"_c_str);
    }
    SetLabel(lable.c_str());
}

void EditorLogsTool::OnFrame()
{
    _isFocused = ImGui::IsWindowFocused();
    if (notReadCount > 0)
    {
        ImGui::PopStyleColor();
        if (_isFocused == true)
        {
            static bool once = false;
            if (once == true)
            {
                notReadCount   = 0;
                _isMessagePush = true;
            }
            else
            {
                once = true;
            }
        }
    } 
    static ImVec2 buttonSize = ImVec2(50, 26);
    static ImVec2 buttonPadding = ImVec2(10, 0);

    if (ImGui::Button("Filter", buttonSize))
        ImGui::OpenPopup("FilterPopup");

    if (ImGui::BeginPopup("FilterPopup"))
    {
        ShowFilter();
        ImGui::EndPopup();
    }

    ImGui::SameLine();
    float windowWidth = ImGui::GetWindowContentRegionMax().x;
    ImGui::SetCursorPosX(windowWidth - buttonSize.x);
    if (ImGui::Button("Clear", buttonSize))
    {
        _editFilter = true;
        _drawLogList.clear();
        engineCore->Logger.LogMessagesClear();
        prevLogCount = 0;
    }

    if (_editFilter)
    {
        _drawLogList.clear();
        const auto& logMessages = engineCore->Logger.GetLogMessages();
        for (auto& [level, message, location] : logMessages)
        {
            if (ReflectFields->LogFilterTable[level] == true)
            {
                _drawLogList.emplace_back(level, message, location);
            }
        }
        _isMessagePush = true;
        _editFilter    = false;
    }

    ImVec2 regionAvail = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("LogScroll", regionAvail, true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGuiListClipper clipper;
    clipper.Begin(_drawLogList.size());
    while (clipper.Step())
    {
        std::string logText;
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            logText.clear();
            auto& [level, message, location] = _drawLogList[i];
            ImGui::PushID(message.c_str());
            logText += message;
            logText += std::format("\n{}, line : {}", location.function_name(), location.line());
            ImGui::PushStyleColor(ImGuiCol_Text, ImGuiHelper::ArrayToImVec4(ReflectFields->LogColorTable[level]));
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4());
            ImGui::InputTextMultiline("##message", &logText, {regionAvail.x, 50}, ImGuiInputTextFlags_ReadOnly);
            ImGuiHelper::HoveredToolTip(u8"우클릭으로 해당 파일로 이동합니다."_c_str);
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                openVSWithFile(location.file_name(), location.line());
            }
            ImGui::PopStyleColor(2);
            ImGui::PopID();
        }   
    }
    if (_isMessagePush)
    {
        ImGui::SetScrollHereY(1.0f);
        _isMessagePush = false;
    }
    ImGui::EndChild();
}

void EditorLogsTool::ResetLogColor()
{
    ReflectFields->LogColorTable[LogLevel::LEVEL_TRACE]   = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_TRACE);
    ReflectFields->LogColorTable[LogLevel::LEVEL_DEBUG]   = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_DEBUG);
    ReflectFields->LogColorTable[LogLevel::LEVEL_INFO]    = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_INFO);
    ReflectFields->LogColorTable[LogLevel::LEVEL_WARNING] = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_WARNING);
    ReflectFields->LogColorTable[LogLevel::LEVEL_ERROR]   = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_ERROR);
    ReflectFields->LogColorTable[LogLevel::LEVEL_FATAL]   = ImGuiHelper::ImVec4ToArray(DEBUG_COLOR_FATAL);
}

void EditorLogsTool::ResetLogFilter()
{
    ReflectFields->LogFilterTable[LogLevel::LEVEL_TRACE] = true;
    ReflectFields->LogFilterTable[LogLevel::LEVEL_DEBUG] = true;
    ReflectFields->LogFilterTable[LogLevel::LEVEL_INFO] = true;
    ReflectFields->LogFilterTable[LogLevel::LEVEL_WARNING] = true;
    ReflectFields->LogFilterTable[LogLevel::LEVEL_ERROR] = true;
    ReflectFields->LogFilterTable[LogLevel::LEVEL_FATAL] = true;
}

void EditorLogsTool::PrintLog(const std::tuple<int, std::string, LogLocation>& log)
{

}

void EditorLogsTool::ShowFilter()
{
    for (int i = 0; i < LogLevel::LEVEL_FATAL; ++i)
    {
        ImGui::BeginHorizontal(i);
        ImGuiHelper::PushStyleCompactToItem(0.6f);
        ImGui::ColorEdit4("##edit", 
                          (float*)&ReflectFields->LogColorTable[i + 1],
                          ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::Text(LogLevel::LogLevelTo_c_str(i + 1));
        ImGui::SetCursorPosX(100.0f);
        if (ImGui::Checkbox("##checkbox",
                            &ReflectFields->LogFilterTable[i + 1]))
        {
            _editFilter = true;
        }
        ImGuiHelper::PopStyleCompact();
        ImGui::EndHorizontal();
    }
    if (ImGui::Button("Reset"))
    {
        ResetLogColor();
        ResetLogFilter();
    }
}

