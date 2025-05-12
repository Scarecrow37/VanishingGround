#include "pch.h"
using namespace Global;

LogLocation::LogLocation(const std::source_location& location) 
{
    LogLocation::EngineLocationInfo& locationInfo = engineCore->LocationInfo;
    {
        auto [iter, result] = locationInfo.lineInfoSet.insert(location.line());
        const uint_least32_t& value = *iter;
        _line                       = &value;
    }
    {
        auto [iter, result] = locationInfo.columnInfoSet.insert(location.column());
        const uint_least32_t& value = *iter;
        _column                     = &value;
    }
    {
        auto [iter, result] = locationInfo.fileInfoSet.insert(location.file_name());
        const std::string& value = *iter;
        _file                    = &value;
    }
    {
        auto [iter, result] = locationInfo.functionInfoSet.insert(location.function_name());
        const std::string& value = *iter;
        _function                = &value;
    }
}

