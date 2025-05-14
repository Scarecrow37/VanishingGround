#pragma once

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

    /* ImGui가 종료될때 한번 호출 */
    virtual void OnEndGui() override;

    /*반드시 호출*/
    virtual void OnTickGui() override;

    /* Begin 호출 전에 항상 호출 (Begin성공 유무 상관 X) */
    virtual void OnPreFrameBegin() override;

    /* Begin 호출 성공 시 호출 */
    virtual void OnPostFrameBegin() override;

    /* End 호출 후에 항상 호출 (Begin성공 유무 상관 X) */
    virtual void OnFrameEnd() override {};
private:
    void ResetLogColor();
    void ResetLogFilter();
    void PrintLog(const std::tuple<int, std::string, LogLocation>& log);
    void ShowFilter();

private:
    size_t prevLogCount = 0;
    size_t notReadCount = 0;
    bool _isMessagePush = false;
    bool _editFilter  = false;
    bool _isWindowFocused = false;
    bool _isWindowHovered = false;
    std::vector<std::tuple<int, std::string, LogLocation>> _drawLogList;

protected:
    REFLECT_FIELDS_BEGIN(EditorTool)
    std::unordered_map<int, std::array<float, 4>> LogColorTable;
    std::unordered_map<int, bool> LogFilterTable;
    REFLECT_FIELDS_END(EditorLogsTool)

};
