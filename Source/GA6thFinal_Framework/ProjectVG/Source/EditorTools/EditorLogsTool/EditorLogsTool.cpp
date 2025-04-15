#include "EditorLogsTool.h"

using namespace u8_literals;
using namespace Global;

static std::filesystem::path GetSettingFilePath()
{
    constexpr const wchar_t* LOG_SETTING_FILE_NAME = L"EditorLogSetting.setting.json";
    std::filesystem::path settingPath = PROJECT_SETTING_PATH;
    settingPath /= LOG_SETTING_FILE_NAME;
    return settingPath;
}

static std::string ReadSettingFile()
{
    std::string data;
    std::ifstream ifs(GetSettingFilePath());
    if (ifs.is_open() == true)
    {
        data = std::string(std::istreambuf_iterator<char>(ifs), {});
    }
    ifs.close();
    return data;
}

static void WriteSettingFile(std::string_view data)
{
    auto settingPath = GetSettingFilePath();
    if (std::filesystem::exists(settingPath) == false)
    {
        std::filesystem::create_directories(settingPath.parent_path());
    }
    std::ofstream ofs(settingPath, std::ios::trunc);
    if (ofs.is_open() == true)
    {
        ofs << data;
    }
    ofs.close();
}

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
    std::string setting = ReadSettingFile();
    if (setting.empty() == false)
    {
        DeserializedReflectFields(setting);
    }
}

void EditorLogsTool::OnEndGui() 
{
    WriteSettingFile(SerializedReflectFields());
}

void EditorLogsTool::OnTickGui() 
{
    const auto& logMessages = engineCore->EngineLogger.GetLogMessages();
    if (prevLogCount < logMessages.size())
    {
        for (size_t i = prevLogCount; i < logMessages.size(); i++)
        {
            auto& [level, message, location] = logMessages[i];
            if (ReflectFields->LogFilterTable[level] == true)
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
            if (ReflectFields->LogFilterTable[level] == true)
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
    auto& [level, message, location] = log;
    ImGui::PushStyleColor(ImGuiCol_Text, ImGuiHelper::ArrayToImVec4(ReflectFields->LogColorTable[level]));
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

