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

    static HRESULT CreateGuid(File::Guid& _id)
    {
        RPC_STATUS  status;
        UUID        uuid;
        const char* charID;

        status = UuidCreate(&uuid);
        if (RPC_S_OK != status)
        {
            return E_FAIL;
        }

        status = UuidToStringA(&uuid, (RPC_CSTR*)&charID);
        if (RPC_S_OK != status)
        {
            return E_FAIL;
        }
        else
        {
            _id = charID;
            return S_OK;
        }
    }

    static void CreateFolder(File::Path _path)
    {
        if (false == std::filesystem::exists(_path))
        {
            if (true == std::filesystem::create_directory(_path))
            {
                OutputLog(L"Succeed Create Folder (" + _path.wstring() + L')');
            }
            else
            {
                OutputLog(L"Failed Created Folder: (" + _path.wstring() + L')');
            }
        }
    }
} // namespace File