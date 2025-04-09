#pragma once

namespace File
{
    using Path = std::filesystem::path;

    enum class EventType
    {
        UNKNOWN = 0,    // 에러
        /* ## CallBack Event ## */
        ADDED,          // 추가
        REMOVED,        // 삭제
        MODIFIED,       // 수정
        RENAMED,        // 이름 변경
        MOVED,          // 이동
    };


    static inline void ThrowSystemError()
    {
        throw std::system_error(GetLastError(), std::system_category());
    }
};