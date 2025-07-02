#include "pch.h"

void CreateMiniDump(_EXCEPTION_POINTERS* pExceptionPointers, int MINIDUMP_TYPE)
{
    constexpr wchar_t dumpPath[] = L"dumps/";
    namespace fs = std::filesystem;
    // 현재 시간 가져오기
    SYSTEMTIME localTime;   
    GetLocalTime(&localTime);

    // 실행 파일 이름 가져오기
    TCHAR moduleName[MAX_PATH];
    GetModuleFileName(nullptr, moduleName, MAX_PATH);
    TCHAR* baseName = _tcsrchr(moduleName, _T('\\'));
    baseName = (baseName != nullptr) ? (baseName + 1) : moduleName; // 파일명 추출
    TCHAR* extension = _tcsrchr(baseName, _T('.'));
    if (extension) {
        *extension = _T('\0'); // 확장자 제거
    }
 
    // 디렉토리 존재 확인 후, 없으면 생성
    if (!fs::exists(dumpPath)) 
    {
         fs::create_directories(dumpPath);  // 디렉토리 생성
    }

    //덤프 파일 이름 생성 (앱 이름 + 날짜 + 시간)
    TCHAR dumpFileName[MAX_PATH];
    _stprintf_s(dumpFileName, _T("%s%s_%04d-%02d-%02d_%02d-%02d-%02d.dmp"),
        dumpPath,
        baseName,
        localTime.wYear, localTime.wMonth, localTime.wDay,
        localTime.wHour, localTime.wMinute, localTime.wSecond);

    // 덤프 파일 생성
    HANDLE hFile = CreateFile(dumpFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) 
    {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ExceptionPointers = pExceptionPointers;
        dumpInfo.ClientPointers = FALSE;

        BOOL success = MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            (_MINIDUMP_TYPE)MINIDUMP_TYPE,
            &dumpInfo,
            nullptr,
            nullptr
        );

        if (!success) 
        {
            //덤프 생성 실패시

        }
        CloseHandle(hFile);
    }
    else 
    {
        //덤프 생성 실패시

    }
}

__callback LONG WINAPI CustomUnhandledExceptionFilter(_In_ _EXCEPTION_POINTERS* pExceptionPointers)
{
#ifdef _DEBUG
    CreateMiniDump(pExceptionPointers, MiniDumpWithFullMemory | MiniDumpWithThreadInfo);
#else //릴리즈
    CreateMiniDump(pExceptionPointers, MiniDumpWithDataSegs | MiniDumpWithThreadInfo);
#endif
    return EXCEPTION_EXECUTE_HANDLER;
}