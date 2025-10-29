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

    static inline void OutputLog(std::wstring_view msg)
    {
#ifdef _DEBUG
        std::wstring debugMsg = std::format(L"FileSystem: {}\n", msg);
        /* 해당 함수는 스레드 세이프 함. */
        OutputDebugString(debugMsg.c_str());
#endif
    }

    static inline void ThrowSystemError()
    {
        throw std::system_error(GetLastError(), std::system_category());
    }

    static bool IsGuid(const std::string& str)
    {
        // "<Guid("95F093BE-764E-476C-8127-C5A3DC277139")>"
        if (str.size() == 36)
        {
            bool isGuid = 
                (str[8] == '-') &&
                (str[13] == '-') && 
                (str[18] == '-') &&
                (str[23] == '-');
            return isGuid;
        }
        return false;
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
    bool CopyStrToClipBoard(std::string_view str);
    bool CopyPathToClipBoard(const File::Path& path);

    /* 해당 경로에 중복 파일이 있을 경우 중복 방지 인덱스를 붙여서 리턴 */
    File::Path GenerateUniquePath(const File::Path& path, int maxIndex = 999);

    enum DirectoryDialogFlag
    {
        DIRECTORY_DIALOG_FLAG_NONE                 = 0,      // 플래그 없음
        DIRECTORY_DIALOG_FLAG_SAVE_FILE            = 1 << 0, // 파일 저장에 대한 플래그
        DIRECTORY_DIALOG_FLAG_OPEN_FILE            = 1 << 1, // 파일 열기에 대한 플래그
        DIRECTORY_DIALOG_FLAG_ALLOW_MULTISELECT    = 1 << 2, // 다중 선택에 대한 플래그
        DIRECTORY_DIALOG_FLAG_PICK_FOLDER          = 1 << 3, // 폴더 선택에 대한 플래그
    };

    struct FileDialogDesc
    {
        HWND                                        Owner = NULL;
        LPCWSTR                                     Title = L"";
        LPCWSTR                                     InitialDirectory = L"";
        LPCWSTR                                     DefaultFileName  = L"";
        std::vector<std::pair<LPCWSTR, LPCWSTR>>    Filters;
        DWORD                                       Flags = 0;
    };

    // 파일 브라우저 열기
    bool ShowOpenFileDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir,
                            std::vector<std::pair<LPCWSTR, LPCWSTR>> filters, OUT std::vector<File::Path>& out);
    bool ShowOpenFileDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir,
                            std::vector<std::pair<LPCWSTR, LPCWSTR>> filters, OUT File::Path& out);
    bool ShowOpenFileDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir,
                             std::vector<std::pair<LPCWSTR, LPCWSTR>> filters, bool allowMultiSelect, OUT std::vector<File::Path>& out);
    bool ShowSaveFileDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir, LPCWSTR defaultName,
                            const std::vector<std::pair<LPCWSTR, LPCWSTR>>& filters, OUT File::Path& out);
    bool ShowOpenFolderDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir, OUT File::Path& out);

    bool ShowFileDialogEx(IN const FileDialogDesc& desc, OUT std::vector<File::Path>& out);


    /// <summary>
    /// 이 컴퓨터의 바탕화면 경로를 가져옵니다. 실패시 "C:"를 반환합니다.
    /// </summary>
    /// <returns>바탕화면 경로</returns>
    std::wstring_view GetDesktopPath();

    // std::wstring을 클립보드에 복사하는 함수
    void SetClipboardText(std::wstring_view text);

    // 클립보드에서 std::wstring을 가져오는 함수
    std::wstring GetClipboardText();
    
    std::time_t GetFileLastWriteTime(const fs::directory_entry& entry);

    class Compare
    {
    public:
        enum SortFlags
        {
            FLAGS_SORT_BY_NONE = 0,      // 정렬 없음
            FLAGS_SORT_BY_TYPE = 1 << 1, // 유형별 정렬
            FLAGS_SORT_BY_NAME = 1 << 2, // 이름순 정렬
            FLAGS_SORT_BY_DATE = 1 << 3, // 날짜순 정렬
        };

    public:
        Compare(int sortFlags = 0) : flags(sortFlags) {}
        ~Compare() = default;
        bool operator()(const fs::directory_entry& a, const fs::directory_entry& b) const
        {
            bool isADir = a.is_directory();
            bool isBDir = b.is_directory();
            if (isADir != isBDir)
            {
                return isADir > isBDir; // 폴더 우선
            }
            if (flags & FLAGS_SORT_BY_NAME)
            {
                return a.path().filename().string() < b.path().filename().string(); // 이름순
            }
            if (flags & FLAGS_SORT_BY_DATE)
            {
                return a.last_write_time() < b.last_write_time(); // 날짜순
            }
            return false;
        }

    private:
        bool CompareByType(const fs::directory_entry& a, const fs::directory_entry& b) const;
        bool CompareByName(const fs::directory_entry& a, const fs::directory_entry& b) const;
        bool CompareByDate(const fs::directory_entry& a, const fs::directory_entry& b) const;

    private:
        int flags = 0; // 정렬 플래그 (예: 이름순, 날짜순 등)
    };
} // namespace File

