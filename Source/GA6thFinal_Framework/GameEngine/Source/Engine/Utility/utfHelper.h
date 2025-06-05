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

//C 스타일 문자열을 도와주는 함수입니다.
namespace CLiteralHelper
{
    //컴파일 타임에 평가가 가능한 문자열을 잘라 반환해줍니다.
    template <std::size_t N, std::size_t START, std::size_t COUNT>
    constexpr std::array<char, COUNT + 1> SliceLiteral(const char (&str)[N])
    {
        static_assert(START + COUNT <= N - 1, "string size overrflow");
        std::array<char, COUNT + 1> result{};
        for (std::size_t i = 0; i < COUNT; ++i)
        {
            result[i] = str[i + START];
        }
        result[COUNT] = '\0';
        return result;
    };
}
