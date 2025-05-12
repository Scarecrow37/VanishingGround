#pragma once

class ELogger
{
    friend class EngineCores;
    ELogger();
    ~ELogger();

public:
    /// <summary>
    /// Log를 남깁니다.
    /// </summary>
    /// <param name="logLevel :">LogLevel에 정의된 constexpr 값을
    /// 사용합니다.</param> <param name="message :">내용</param> <param
    /// name="location :">콜러의 정보를 위한 매개변수입니다. 기본값
    /// 사용해야합니다.</param>
    void Log(
        int logLevel, 
        std::string_view message,
        const LogLocation location = std::source_location::current());

    /// <summary>
    /// 프로그램 실행중 생성된 모든 로그 메시지를 반환합니다.
    /// </summary>
    const std::vector<std::tuple<int, std::string, LogLocation>>&
    GetLogMessages()
    {
        return _logMessages;
    }

    /// <summary>
    /// 프로그램 실행중 생성된 로그 중, 특정 레벨의 로그만 반환합니다.
    /// </summary>
    /// <param name="logLevelFilter :">원하는 로그 레벨</param>
    /// <returns>std::ranges::views::filter_view 객체를 반환합니다.</returns>
    inline auto GetLogMessages(int logLevelFilter);

    /// <summary>
    /// 프로그램 실행중 생성된 로그 기록을 정리합니다. 0을 전달하면 모든 로그를
    /// 정리합니다.
    /// </summary>
    /// <param name="logLevel :">지울 로그 레벨</param>
    void LogMessagesClear(int logLevel = NULL);

private:
    std::vector<std::tuple<int, std::string, LogLocation>>
        _logMessages;
};

inline auto ELogger::GetLogMessages(int logLevelFilter)
{
    auto filter =
        _logMessages |
        std::ranges::views::filter(
            [logLevelFilter](std::tuple<int, std::string, LogLocation>& log) {
                auto& [level, message, location] = log;
                return logLevelFilter == level;
            });
    return filter;
}
