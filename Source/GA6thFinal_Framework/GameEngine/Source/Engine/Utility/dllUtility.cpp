#include "pch.h"
#include <filesystem>

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
    bool result = false;

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = NULL;
    sei.lpVerb = L"runas";
    sei.lpFile = BatchFilePath.data();
    sei.lpParameters = L"";
    sei.nShow = SW_SHOWNORMAL;

    if (ShellExecuteExW(&sei))
    {
        // Wait for the process to finish
        if (sei.hProcess != 0)
        {
            WaitForSingleObject(sei.hProcess, INFINITE);
            DWORD exitCode;
            if (GetExitCodeProcess(sei.hProcess, &exitCode))
            {
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
                if (pExitCodeOut)
                {
                    *pExitCodeOut = GetLastError(); // Store the error code on failure
                }
                result = false;
            }

            // Close the process handle
            CloseHandle(sei.hProcess);
        }      
    }
    else
    {
        if (pExitCodeOut)
        {
            // For example, ERROR_CANCELLED (1223) if the user clicks "No" on the UAC prompt
            *pExitCodeOut = GetLastError();
        }
        result = false;
    }
    return result;
}

