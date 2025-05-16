#pragma once
// 파일 열기 대화 상자
#include <commdlg.h>
#include <shlobj.h>

// uuid
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

    enum DirectoryBrowserFlag
    {
        DIRECTORY_BROWSER_FLAG_NONE                 = 0,      // 플래그 없음
        DIRECTORY_BROWSER_FLAG_SAVE_FILE            = 1 << 0, // 파일 저장에 대한 플래그
        DIRECTORY_BROWSER_FLAG_OPEN_FILE            = 1 << 1, // 파일 열기에 대한 플래그
        DIRECTORY_BROWSER_FLAG_ALLOW_MULTISELECT    = 1 << 2, // 다중 선택에 대한 플래그
        DIRECTORY_BROWSER_FLAG_PICK_FOLDER          = 1 << 3, // 폴더 선택에 대한 플래그
    };
    using DirectoryBrowserFlags = DWORD;

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

    /* 폴더를 만드는 함수. 중복 시 false 리턴 */ 
    bool CreateFolder(const File::Path& path);
    /* 폴더를 만드는 함수. processDup를 true로 할 시 중복 시 GenerateUniquePath함수로 고유 이름을 만들어 생성 */ 
    bool CreateFolderEx(const File::Path& path /* 경로 */, bool processDup = false /* 중복 처리 */);

    /* 파일을 여는 함수 */
    bool OpenFile(const File::Path& path);

    /*  파일을 삭제하는 함수 */
    bool RemoveFile(const File::Path& path);

    /* 파일을 복사하는 함수. (폴더는 X) */
    bool CopyFileFromTo(const File::Path& from, File::Path to);

    /* 클립보드에 문자열을 복사해주는 함수 */
    bool CopyPathToClipBoard(const File::Path& path);
    /* 해당 경로에 중복 파일이 있을 경우 중복 방지 인덱스를 붙여서 리턴 */
    File::Path GenerateUniquePath(const File::Path& path, int maxIndex = 999);

    // 파일 브라우저 열기
    bool ShowOpenFileBrowser(HWND owner, LPCWSTR title, LPCWSTR initialDir,
                             std::vector<std::pair<LPCWSTR, LPCWSTR>> filters, bool allowMultiSelect, OUT std::vector<File::Path>& out);
    bool ShowSaveFileBrowser(HWND owner, LPCWSTR title, LPCWSTR initialDir, LPCWSTR defaultName, OUT File::Path& out);
    bool ShowOpenFolderBrowser(HWND owner, LPCWSTR title, LPCWSTR initialDir, OUT File::Path& out);

    bool ShowSelectDirectoryBrowserEx(HWND owner, LPCWSTR title, LPCWSTR initialDirectory, LPCWSTR initialFileName,
                                                           std::vector<std::pair<LPCWSTR, LPCWSTR>> filters,
                                                           DirectoryBrowserFlags flags,
                                                           OUT std::vector<File::Path>& out);
} // namespace File

