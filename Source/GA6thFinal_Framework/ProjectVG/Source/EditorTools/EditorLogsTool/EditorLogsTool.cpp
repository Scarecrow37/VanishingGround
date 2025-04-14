#include "EditorLogsTool.h"

using namespace u8_literals;
using namespace Global;

EditorLogsTool::EditorLogsTool()
{
    SetLabel(u8"로그"_c_str);
    ResetLogColor();
    ResetLogFilter();
}

EditorLogsTool::~EditorLogsTool()
{

}

void EditorLogsTool::OnStartGui()
{
    
}

void EditorLogsTool::OnTickGui() 
{
    const auto& logMessages = engineCore->EngineLogger.GetLogMessages();
    if (prevLogCount < logMessages.size())
    {
        for (size_t i = prevLogCount; i < logMessages.size(); i++)
        {
            auto& [level, message, location] = logMessages[i];
            if (LogFilterTable[level] == true)
            {
                _drawLogList.emplace_back(level, message, location);   
            }            
        }
        prevLogCount = logMessages.size();
        _isMessagePush = true;
        if (GetVisible() == false)
        {
            SetVisible(true);
        }
    }
}

void EditorLogsTool::OnPreFrame() 
{

}

void EditorLogsTool::OnFrame()
{
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
        engineCore->EngineLogger.LogMessagesClear();
        prevLogCount = 0;
    }

    if (_editFilter)
    {
        _drawLogList.clear();
        const auto& logMessages = engineCore->EngineLogger.GetLogMessages();
        for (auto& [level, message, location] : logMessages)
        {
            if (LogFilterTable[level] == true)
            {
                _drawLogList.emplace_back(level, message, location);
            }
        }
        _isMessagePush = true;
        _editFilter    = false;
    }

    ImGui::BeginChild("LogScroll", ImGui::GetContentRegionAvail(), true,ImGuiWindowFlags_HorizontalScrollbar);
    ImGuiListClipper clipper;
    clipper.Begin(_drawLogList.size());
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
        {
            PrintLog(_drawLogList[i]);
            ImGui::Text("");
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
    LogColorTable[LogLevel::LEVEL_TRACE] = DEBUG_COLOR_TRACE;
    LogColorTable[LogLevel::LEVEL_DEBUG] = DEBUG_COLOR_DEBUG;
    LogColorTable[LogLevel::LEVEL_INFO] = DEBUG_COLOR_INFO;
    LogColorTable[LogLevel::LEVEL_WARNING] = DEBUG_COLOR_WARNING;
    LogColorTable[LogLevel::LEVEL_ERROR] = DEBUG_COLOR_ERROR;
    LogColorTable[LogLevel::LEVEL_FATAL] = DEBUG_COLOR_FATAL;
}

void EditorLogsTool::ResetLogFilter()
{
    LogFilterTable[LogLevel::LEVEL_TRACE] = true;
    LogFilterTable[LogLevel::LEVEL_DEBUG] = true;
    LogFilterTable[LogLevel::LEVEL_INFO] = true;
    LogFilterTable[LogLevel::LEVEL_WARNING] = true;
    LogFilterTable[LogLevel::LEVEL_ERROR] = true;
    LogFilterTable[LogLevel::LEVEL_FATAL] = true;
}

void EditorLogsTool::PrintLog(const std::tuple<int, std::string, LogLocation>& log)
{
    auto& [level, message, location] = log;
    ImGui::PushStyleColor(ImGuiCol_Text, LogColorTable[level]);
    ImGui::TextUnformatted(message.c_str());
    ImGui::Text(std::format("{}, line : {}", location.function_name(), location.line()).c_str());
    ImGui::PopStyleColor();
}

void EditorLogsTool::ShowFilter()
{
    for (int i = 0; i < LogLevel::LEVEL_FATAL; ++i)
    {
        ImGui::BeginHorizontal(i);
        ImGuiHelper::PushStyleCompactToItem(0.6f);
        ImGui::ColorEdit4("##edit", (float*)&LogColorTable[i + 1], ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::Text(LogLevel::LogLevelTo_c_str(i + 1));
        ImGui::SetCursorPosX(100.0f);
        if (ImGui::Checkbox("##checkbox", &LogFilterTable[i + 1]))
        {
            _editFilter = true;
        }
        ImGuiHelper::PopStyleCompact();
        ImGui::EndHorizontal();
    }
}

