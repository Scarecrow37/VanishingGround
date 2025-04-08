#pragma once
#include "UmFramework.h"

constexpr ImVec4 DEBUG_COLOR_TRACE = ImVec4(0.63f, 0.63f, 0.63f, 1.0f);  // 연한 회색
constexpr ImVec4 DEBUG_COLOR_DEBUG = ImVec4(0.0f, 0.75f, 1.0f, 1.0f);    // 밝은 파랑
constexpr ImVec4 DEBUG_COLOR_INFO = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);     // 흰색
constexpr ImVec4 DEBUG_COLOR_WARNING = ImVec4(1.0f, 0.84f, 0.0f, 1.0f);    // 노란색
constexpr ImVec4 DEBUG_COLOR_ERROR = ImVec4(1.0f, 0.27f, 0.0f, 1.0f);    // 빨간색
constexpr ImVec4 DEBUG_COLOR_FATAL = ImVec4(0.55f, 0.0f, 0.0f, 1.0f);    // 진한 빨강

class EditorLogsTool : public EditorTool
{
public:
    EditorLogsTool();
    ~EditorLogsTool();

protected:
    /* ImGui가 초기화된 후 한번 호출 */
    virtual void OnStartGui() override;

    /* Begin 호출 전에 항상 호출 (Begin성공 유무 상관 X) */
    virtual void OnPreFrame() override {};

    /* Begin 호출 성공 시 호출 */
    virtual void OnFrame() override;

    /* End 호출 후에 항상 호출 (Begin성공 유무 상관 X) */
    virtual void OnPostFrame() override {};
private:
    void ResetLogColor();
    void ResetLogFilter();
    void PrintLog(const std::tuple<int, std::string, std::source_location>& log);
    void ShowFilter();

    std::unordered_map<int, ImVec4> LogColorTable;
    std::unordered_map<int, bool> LogFilterTable;
};