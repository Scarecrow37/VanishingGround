#pragma once

class LogOutput : public Component
{
    USING_PROPERTY(LogOutput)
    inline static constexpr ImVec4 DEBUG_COLOR_TRACE   = ImVec4(0.63f, 0.63f, 0.63f, 1.0f); // 연한 회색
    inline static constexpr ImVec4 DEBUG_COLOR_DEBUG   = ImVec4(0.0f, 0.75f, 1.0f, 1.0f);   // 밝은 파랑
    inline static constexpr ImVec4 DEBUG_COLOR_INFO    = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // 흰색
    inline static constexpr ImVec4 DEBUG_COLOR_WARNING = ImVec4(1.0f, 0.84f, 0.0f, 1.0f);   // 노란색
    inline static constexpr ImVec4 DEBUG_COLOR_ERROR   = ImVec4(1.0f, 0.27f, 0.0f, 1.0f);   // 빨간색
    inline static constexpr ImVec4 DEBUG_COLOR_FATAL   = ImVec4(0.55f, 0.0f, 0.0f, 1.0f);   // 진한 빨강
public:
    LogOutput();
    ~LogOutput() override;

public:
    REFLECT_PROPERTY(
        TraceColor, 
        DebugColor,
        InfoColor,
        WarningColor,
        ErrorColor,
        FatalColor)

    SETTER(Color, TraceColor)
    {
        std::memcpy(ReflectFields->LogColorTable[LogLevel::LEVEL_TRACE].data(), &value.x, sizeof(Color));
    }
    GETTER(Color, TraceColor) { return Color(ReflectFields->LogColorTable[LogLevel::LEVEL_TRACE].data()); }
    PROPERTY(TraceColor)

    SETTER(Color, DebugColor)
    {
        std::memcpy(ReflectFields->LogColorTable[LogLevel::LEVEL_DEBUG].data(), &value.x, sizeof(Color));
    }
    GETTER(Color, DebugColor) { return Color(ReflectFields->LogColorTable[LogLevel::LEVEL_DEBUG].data()); }
    PROPERTY(DebugColor)

    SETTER(Color, InfoColor)
    {
        std::memcpy(ReflectFields->LogColorTable[LogLevel::LEVEL_INFO].data(), &value.x, sizeof(Color));
    }
    GETTER(Color, InfoColor) { return Color(ReflectFields->LogColorTable[LogLevel::LEVEL_INFO].data()); }
    PROPERTY(InfoColor)

    SETTER(Color, WarningColor)
    {
        std::memcpy(ReflectFields->LogColorTable[LogLevel::LEVEL_WARNING].data(), &value.x, sizeof(Color));
    }
    GETTER(Color, WarningColor) { return Color(ReflectFields->LogColorTable[LogLevel::LEVEL_WARNING].data()); }
    PROPERTY(WarningColor)

    SETTER(Color, ErrorColor)
    {
        std::memcpy(ReflectFields->LogColorTable[LogLevel::LEVEL_ERROR].data(), &value.x, sizeof(Color));
    }
    GETTER(Color, ErrorColor) { return Color(ReflectFields->LogColorTable[LogLevel::LEVEL_ERROR].data()); }
    PROPERTY(ErrorColor)

    SETTER(Color, FatalColor)
    {
        std::memcpy(ReflectFields->LogColorTable[LogLevel::LEVEL_FATAL].data(), &value.x, sizeof(Color));
    }
    GETTER(Color, FatalColor) { return Color(ReflectFields->LogColorTable[LogLevel::LEVEL_FATAL].data()); }
    PROPERTY(FatalColor)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::unordered_map<int, std::array<float, 4>> LogColorTable;
    REFLECT_FIELDS_END(LogOutput)

    void Update() override;

private:
    void DrawShowLog();
    void DrawLogSetting();

    bool  _isShowLog;
    bool  _isShowSetting;

};
