﻿#include "pch.h"

static std::pair<std::string, std::string> GetCurrentTimestamp() 
{
    using namespace std::chrono;
    auto now = system_clock::now();
    auto time_t_now = system_clock::to_time_t(now);
    auto local_tm = *std::localtime(&time_t_now);

    return std::make_pair(
        std::format("{:02}_{:02}_{:02}",
        local_tm.tm_year % 100, // yy
        local_tm.tm_mon + 1,    // mm (0-based index)
        local_tm.tm_mday        // dd
        ), 
        std::format("{:02}:{:02}:{:02}",
        local_tm.tm_hour,       // HH
        local_tm.tm_min,        // MM
        local_tm.tm_sec)        // SS
    );     
}

EEngineLogger::EEngineLogger()
{
}

EEngineLogger::~EEngineLogger() = default;

void EEngineLogger::Log(int logLevel, std::string_view message, const std::source_location location)
{
    auto [day, time] = GetCurrentTimestamp();
    std::string logMessage = std::format("[{}] {}: {}", time, LogLevel::LogLevelTo_c_str(logLevel), message.data());

    if (LogLevel::IsLogLevel(logLevel))
    {
        _logMessages.emplace_back(logLevel, logMessage, location);
    }

}

void EEngineLogger::LogMessagesClear(int logLevel)
{
    using namespace LogLevel;
    if (logLevel == NULL)
    {
        _logMessages.clear();
    }
    else if (LogLevel::IsLogLevel(logLevel))
    {
        std::erase_if(_logMessages, [logLevel](std::tuple<int, std::string, std::source_location>& log)
            {
                auto& [level, message, location] = log;
                return level == logLevel;
            });
    }
}


