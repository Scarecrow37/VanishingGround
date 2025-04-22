#pragma once
namespace LogLevel
{
    //가장 상세한 로그 레벨로, 애플리케이션의 실행 흐름과 디버깅 정보를 상세히 기록한다. 주로 디버깅 시에 사용된다.
    constexpr int LEVEL_TRACE = 1;

    /*
       디버깅 목적으로 사용되며, 개발 단계에서 상세한 정보를 기록한다.
       애플리케이션의 내부 동작을 이해하고 문제를 분석하는 데 도움을 준다.
    */
    constexpr int LEVEL_DEBUG = 2;

    /*
       정보성 메시지를 기록한다.
       애플리케이션의 주요 이벤트나 실행 상태에 대한 정보를 전달한다.
    */
    constexpr int LEVEL_INFO = 3;

    /*
        경고성 메시지를 기록한다.
        예상치 못한 문제나 잠재적인 오류 상황을 알리는 메시지이다.
        애플리케이션이 정상적으로 동작하지만 주의가 필요한 상황을 알려준다.
    */
    constexpr int LEVEL_WARNING = 4;

    /*
        오류 메시지를 기록한다.
        심각한 문제 또는 예외 상황을 나타내며, 애플리케이션의 정상적인 동작에 영향을 미칠 수 있는 문제를 알린다.
    */
    constexpr int LEVEL_ERROR = 5;

    /*
        가장 심각한 오류 메시지를 기록한다.
        애플리케이션의 동작을 중단시킬 수 있는 치명적인 오류를 나타낸다.
        일반적으로 이러한 오류는 복구가 불가능하거나 매우 어려운 상황을 의미한다.
    */
    constexpr int LEVEL_FATAL = 6;

    /// <summary>
    /// 로그 레벨 값을 문자열로 반환해줍니다.
    /// </summary>
    /// <param name="logLevel :">로그 레벨</param>
    /// <returns>해당 로그 레벨의 문자열 값</returns>
    constexpr const char* LogLevelTo_c_str(int logLevel)
    {
        switch (logLevel)
        {
        case LEVEL_TRACE:
            return "Trace";
        case LEVEL_DEBUG:
            return "Debug";
        case LEVEL_INFO:
            return "Info";
        case LEVEL_WARNING:
            return "Warning";
        case LEVEL_ERROR:
            return "Error";
        case LEVEL_FATAL:
            return "Fatal";
        default:
            return NULL_STR;
        }
    }

    /// <summary>
    /// 이 int 값이 유효한 로그 레벨인지 확인합니다.
    /// </summary>
    constexpr bool IsLogLevel(int logLevel)
    {
        return LogLevel::LEVEL_TRACE <= logLevel && logLevel <= LogLevel::LEVEL_FATAL;
    }
}

// 로그 함수를 호출한 파일 정보를 저장하기 위한 구조체 (source_location 깊은 복사 용)
struct LogLocation
{
public:
    // 중복 메모리 생성 방지용
    struct EngineLocationInfo
    {
        std::unordered_set<std::string>    fileInfoSet;
        std::unordered_set<std::string>    functionInfoSet;
        std::unordered_set<uint_least32_t> lineInfoSet;
        std::unordered_set<uint_least32_t> columnInfoSet;
    };
   
    LogLocation(const std::source_location& location);
    LogLocation(const LogLocation& rhs) 
    { 
        *this = rhs;
    }
    LogLocation& operator=(const LogLocation& rhs)
    {
        if (this == &rhs)
            return *this;

        _line     = rhs._line;
        _column   = rhs._column;
        _file     = rhs._file;
        _function = rhs._function;

        return *this;
    }

    ~LogLocation() = default;
    constexpr uint_least32_t         line() const noexcept { return *_line; }
    constexpr uint_least32_t         column() const noexcept { return *_column; }
    _NODISCARD constexpr const char* file_name() const noexcept
    {
        return _file->c_str();
    }
    _NODISCARD constexpr const char* function_name() const noexcept
    {
        return _function->c_str();
    }

private:
    const uint_least32_t* _line     = nullptr;
    const uint_least32_t* _column   = nullptr;
    const std::string*    _file     = nullptr;
    const std::string*    _function = nullptr;
};