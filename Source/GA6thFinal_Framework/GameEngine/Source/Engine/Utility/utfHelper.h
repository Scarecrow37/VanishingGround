#pragma once

namespace u8_literals
{
    //u8 타입 const char* 캐스팅용 리터럴
    const char* operator"" _c_str(const char8_t* str, std::size_t len);

    //wchar_t 타입 string 캐스팅용 리터럴
    std::string operator"" _to_string(const wchar_t* str, std::size_t len);

    //char 타입 wstring 캐스팅용 리터럴
    std::wstring operator"" _to_wstring(const char* str, std::size_t len);
}

std::wstring U8ToWString(std::string_view utf8_str);

std::string WStringToU8(std::wstring_view wstring);

