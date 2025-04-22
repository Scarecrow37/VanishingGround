#include "pch.h"
#include "FileHelper.h"

namespace File
{
    HRESULT CreateGuid(File::Guid& _id)
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
    void CreateFolder(const File::Path& _path) 
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
    bool OpenFile(const File::Path& path)
    {
        File::Path AbsPath = std::filesystem::absolute(path);

        HINSTANCE hr = ShellExecuteW(NULL,         // 부모 윈도우 핸들
                                     L"open",      // 작업(“open”, “edit”, “print” 등)
                                     AbsPath.c_str(), // 실행할 파일 경로
                                     NULL,         // 커맨드라인 인자
                                     NULL, // 기본 디렉터리
                                     SW_SHOWNORMAL // 창 표시 방식
        );
        if ((int)hr <= 32)
        {
            int debugLevel = UmFileSystem.GetDebugLevel();
            if (debugLevel > 0)
                OutputLog(L"Failed Open File: (" + path.wstring() + L')');
            return false;
        }
        else
        {
            return true;
        }
    }

    bool RemoveFile(const File::Path& path)
    {
        if (true == std::filesystem::exists(path))
        {
            if (std::filesystem::is_directory(path))
            {
                return std::filesystem::remove_all(path); // 디렉토리 삭제
            }
            else
            {
                return std::filesystem::remove(path); // 파일 삭제
            }
        }
        return false;
    }
}
