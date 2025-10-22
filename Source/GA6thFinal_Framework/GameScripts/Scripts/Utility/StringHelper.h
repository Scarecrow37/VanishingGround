#pragma once

namespace StringHelper
{
    // 문자열 좌우 공백 제거 함수
    std::string Trim(std::string_view str);

    int StringToInt(std::string_view str);

    bool StringToInt(std::string_view str, int& outValue);

    std::vector<std::string> ParseCSVToString(std::string_view paramStr); // ex) a, b, c
    std::vector<int> ParseCSVToInt(std::string_view paramStr); // ex) 1, 5, 2
}
