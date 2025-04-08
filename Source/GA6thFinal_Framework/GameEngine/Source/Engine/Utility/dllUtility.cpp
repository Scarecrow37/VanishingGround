#include "pch.h"

std::vector<std::string> dllUtility::GetDLLFuntionNameList(HMODULE dllModule)
{
    std::vector<std::string> out;

    if (!dllModule) return out;  

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)dllModule;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) return out;

    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)dllModule + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) return out;

    DWORD exportDirectoryRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    if (!exportDirectoryRVA) return out;

    PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)dllModule + exportDirectoryRVA);

    DWORD* namesRVA = (DWORD*)((BYTE*)dllModule + exportDirectory->AddressOfNames);
    if (!namesRVA)
    {
        return out;
    }

    for (DWORD i = 0; i < exportDirectory->NumberOfNames; i++) 
    {
        const char* functionName = (const char*)((BYTE*)dllModule + namesRVA[i]); 
        if (!IsBadReadPtr(functionName, 1)) 
        {
            out.push_back(functionName);
        }
    }

    return out;
}

bool dllUtility::RunBatchFile(const std::wstring_view BatchFilePath, DWORD* pExitCodeOut)
{
    using namespace std::string_literals;
    bool result = false;
    std::wstring command = BatchFilePath.data();

    STARTUPINFOW  si = { sizeof(STARTUPINFOW) };
    PROCESS_INFORMATION pi{};
    if (CreateProcessW(
        NULL,                           // 애플리케이션 경로 (NULL이면 첫 번째 파라미터가 명령어로 간주됨)
        (LPWSTR)command.c_str(),        // 명령어 (배치 파일 경로)
        NULL,                           // 프로세스 보안 속성
        NULL,                           // 쓰레드 보안 속성
        FALSE,                          // 자식 프로세스가 부모 프로세스를 상속할지 여부
        0,                              // 생성 플래그 (예: CREATE_NEW_CONSOLE)
        NULL,                           // 환경 변수
        NULL,                           // 현재 작업 디렉토리
        &si,                            // STARTUPINFO 구조체
        &pi                             // PROCESS_INFORMATION 구조체
    )) 
    {
        // 프로세스 종료 대기
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
            if (exitCode == 0) 
            {
                result = true;
            }
            else 
            {
                result = false;
            }
            if (pExitCodeOut)
            {
                *pExitCodeOut = exitCode;
            }
        }
        else 
        {
            result = false;
        }

        // 프로세스 핸들 닫기
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else 
    {
        if (pExitCodeOut)
        {
            *pExitCodeOut = NULL;
        }
        return result;
    }
    return result;
}

