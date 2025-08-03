#pragma once

namespace EditorIcon
{
    inline constexpr std::string UnicodeToUTF8(unsigned int codepoint)
    {
        std::string out;

        if (codepoint <= 0x7F)
        {
            out += static_cast<char>(codepoint);
        }
        else if (codepoint <= 0x7FF)
        {
            out += static_cast<char>(0xC0 | (codepoint >> 6));
            out += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        else if (codepoint <= 0xFFFF)
        {
            out += static_cast<char>(0xE0 | (codepoint >> 12));
            out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        else if (codepoint <= 0x10FFFF)
        {
            out += static_cast<char>(0xF0 | (codepoint >> 18));
            out += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            out += static_cast<char>(0x80 | (codepoint & 0x3F));
        }

        return out;
    }

    inline constexpr std::array<char, 5> UnicodeToUTF8Array(unsigned codepoint)
    {
        std::array<char, 5> out = {};
        if (codepoint <= 0x7F)
        {
            out[0] = static_cast<char>(codepoint);
        }
        else if (codepoint <= 0x7FF)
        {
            out[0] = static_cast<char>(0xC0 | (codepoint >> 6));
            out[1] = static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        else if (codepoint <= 0xFFFF)
        {
            out[0] = static_cast<char>(0xE0 | (codepoint >> 12));
            out[1] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            out[2] = static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        else // 최대 0x10FFFF
        {
            out[0] = static_cast<char>(0xF0 | (codepoint >> 18));
            out[1] = static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            out[2] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            out[3] = static_cast<char>(0x80 | (codepoint & 0x3F));
        }
        // out[4]는 기본으로 '\0'
        return out;
    }

    constexpr const char* ICON_FILE                 = "\xef\x85\x9b"; // f15b
    constexpr const char* ICON_FILE_COPY            = "\xef\xa4\x8d"; // f24d
    constexpr const char* ICON_FILE_PASTE           = "\xef\x83\xaa"; // f0ea
    constexpr const char* ICON_FILE_SAVE            = "\xef\x83\x87"; // f0c7
    constexpr const char* ICON_FILE_CODE            = "\xef\x87\x89"; // f1c9
    constexpr const char* ICON_FILE_AUDIO           = "\xef\x87\x87"; // f1c7
    constexpr const char* ICON_FILE_IMAGE           = "\xef\x87\x85"; // f1c5
    constexpr const char* ICON_FILE_DOCS            = "\xef\x85\x9c"; // f15c

    constexpr const char* ICON_FOLDER               = "\xef\x81\xbb"; // 
    constexpr const char* ICON_FOLDER_OPEN          = "\xef\x81\xbc"; // f07c

    constexpr const char* ICON_HELP                 = "\xef\x81\x99"; // f059
    constexpr const char* ICON_QUESTION             = "\xef\x81\x99"; // f059
    constexpr const char* ICON_EDIT                 = "\xef\x83\x84"; // f044
    constexpr const char* ICON_LIST                 = "\xEF\x80\xA2"; // f0a2  리스트-alt
    constexpr const char* ICON_IMAGE                = "\xEF\x80\xBE"; // f03e  이미지
    constexpr const char* ICON_IMAGES               = "\xEF\x8C\x82"; // f302  이미지s
    constexpr const char* ICON_STAR                 = "\xEF\x80\x85"; // f005  별
    constexpr const char* ICON_HEART                = "\xEF\x80\x84"; // f004  하트
    constexpr const char* ICON_THUMBS_UP            = "\xEF\x85\xA4"; // f164  따봉
    constexpr const char* ICON_KEYBOARD             = "\xef\x84\x9c"; // f11c
    constexpr const char* ICON_SQUARE               = "\xef\x83\x88"; // f0c8
    constexpr const char* ICON_CIRCLE               = "\xef\x84\x91"; // f111
    constexpr const char* ICON_X                    = "\xEF\x80\x8D"; // f00d  X

    constexpr const char* ICON_VISIBLE              = "\xef\x83\xae"; // f06e (eye)
    constexpr const char* ICON_INVISIBLE            = "\xef\x83\xb0"; // f070 (eye-slash)

    constexpr const char* ICON_CIRCLE_ARROW_LEFT    = "\xef\x8d\x99"; // f359
    constexpr const char* ICON_CIRCLE_ARROW_RIGHT   = "\xef\x8d\x9a"; // f35a
    constexpr const char* ICON_CIRCLE_ARROW_UP      = "\xef\x8d\x9b"; // f35b
    constexpr const char* ICON_CIRCLE_ARROW_DOWN    = "\xef\x8d\x9c"; // f35c

    constexpr const char* ICON_BELL_ON              = "\xef\x83\xb3"; // f0f3
    constexpr const char* ICON_BELL_OFF             = "\xef\x87\xb6"; // f1f6

    constexpr const char* ICON_PLAY                 = "\xef\x85\x84";
    constexpr const char* ICON_STOP                 = "\xef\x8a\x8d";
    constexpr const char* ICON_PAUSE                = "\xef\x8a\x8b";
};



//에디터에서 사용되는 프로세스들을 도와주는 이벤트 함수입니다.
struct ProcessHelper
{
    /// <summary>
    /// 비주얼 스튜디오의 실행 여부를 확인합니다.
    /// </summary>
    /// <param name="outExeFilePath :">비주얼 스튜디오 exe 파일 경로</param>
    /// <returns>실행 여부</returns>
    static bool IsVisualStudio(std::string& outExeFilePath);
};