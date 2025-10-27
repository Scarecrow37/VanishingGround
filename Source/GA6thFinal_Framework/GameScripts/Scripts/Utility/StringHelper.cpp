#include "pchScripts.h"
#include "StringHelper.h"

namespace StringHelper
{
    std::string Trim(std::string_view str)
    {
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end   = str.find_last_not_of(" \t\n\r");
        if (start == std::string::npos)
            return "";
        std::string result(str);
        return result.substr(start, end - start + 1);
    }
    int StringToInt(std::string_view str) 
    {
        try
        {
            return std::stoi(Trim(str));
        }
        catch (...)
        {
            assert(false && "문자열을 정수로 변환하는데 실패했습니다.");
            return 0;
        }
        return 0;
    }
    bool StringToInt(std::string_view str, int& outValue) 
    {
        try
        {
            outValue = std::stoi(Trim(str));
            return true;
        }
        catch (...)
        {
            assert(false && "문자열을 정수로 변환하는데 실패했습니다.");
            return false;
        }
    }
    std::vector<std::string> ParseCSVToString(std::string_view paramStr) 
    {
        std::vector<std::string> result;
        std::stringstream ss(paramStr.data());
        std::string       token;

        while (std::getline(ss, token, ',')) // ','로 구분
        {
            result.push_back(Trim(token));
        }
        return result;
    }
    std::vector<int> ParseCSVToInt(std::string_view paramStr) 
    {
        std::vector<int>         result;
        std::stringstream        ss(paramStr.data());
        std::string              token;
        while (std::getline(ss, token, ',')) // ','로 구분
        {
            int value = 0;
            if (StringToInt(Trim(token), value))
            {
                result.push_back(value);
            }
        }
        return result;
    }
} // namespace StringHelper