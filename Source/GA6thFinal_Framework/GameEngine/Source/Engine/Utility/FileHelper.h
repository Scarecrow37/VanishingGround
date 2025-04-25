#pragma once
#include <rpc.h>
#pragma comment(lib, "Rpcrt4.lib")

namespace File
{
    namespace fs = std::filesystem;

    enum class EventType
    {
        UNKNOWN = 0, // 에러
        /* ## CallBack Event ## */
        ADDED,    // 추가
        REMOVED,  // 삭제
        MODIFIED, // 수정
        RENAMED,  // 이름 변경
        MOVED,    // 이동
    };

    static void OutputLog(const std::wstring& msg)
    {
#ifdef _DEBUG
        std::wstring debugMsg = L"FileSystem: " + msg + L'\n';
        /* 해당 함수는 스레드 세이프 함. */
        OutputDebugString(debugMsg.c_str());
#endif
    }

    static inline void ThrowSystemError()
    {
        throw std::system_error(GetLastError(), std::system_category());
    }

    HRESULT CreateGuid(File::Guid& _id);

    bool CreateFolder(const File::Path& path);
    bool CreateFolderEx(const File::Path& path, /* 경로 */
        bool processDup = false /* 중복 처리 */
    );

    bool OpenFile(const File::Path& path);

    bool RemoveFile(const File::Path& path);

    bool CopyFileFromTo(const File::Path& from, File::Path to);

    bool CopyPathToClipBoard(const File::Path& path);

    File::Path GenerateUniquePath(const File::Path& path, unsigned int maxIndex = 999);
} // namespace File