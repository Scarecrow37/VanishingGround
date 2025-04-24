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
    bool CreateFolder(const File::Path& path) 
    {
        if (false == std::filesystem::exists(path))
        {
            bool check = std::filesystem::create_directory(path);
            if (true == check)
            {
                OutputLog(L"Succeed Create Folder (" + path.wstring() + L')');
            }
            else
            {
                OutputLog(L"Failed Created Folder: (" + path.wstring() + L')');
            }
            return check;
        }
        return false;
    }

    bool CreateFolderEx(const File::Path& path, bool processDup) 
    {
        if (false == std::filesystem::exists(path))
        {
            return CreateFolder(path);
        }
        else if (true == processDup)
        {
            return CreateFolder(GenerateUniquePath(path));
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
    bool CopyPathToClipBoard(const File::Path& path)
    {
        // 클립보드 열고 비우기
        if (TRUE == OpenClipboard(nullptr) && TRUE == EmptyClipboard())
        {
            if (TRUE == EmptyClipboard())
            {
                auto str  = path.string(); // UTF-8 문자열로 변환
                auto size = (str.size() + 1) * sizeof(char);

                // 사용자 정의 포맷 등록 (고유 문자열 사용)
                UINT format = RegisterClipboardFormatA("MyApp_CustomDataFormat");

                // 글로벌 메모리 할당
                HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, size);
                if (NULL == hGlob)
                {
                    CloseClipboard();
                    return false;
                }

                void* pData = GlobalLock(hGlob);
                memcpy(pData, str.c_str(), size);
                GlobalUnlock(hGlob);

                // CF_TEXT는 ANSI지만, UTF-8도 보통 잘 작동함
                SetClipboardData(CF_TEXT, hGlob);

                CloseClipboard();
            }
        }
        return false;
    }
    File::Path GenerateUniquePath(const File::Path& path, unsigned int maxIndex)
    {
        // 중복일 시 뒤에 () 붙여서 생성
        File::Path name      = path.stem();
        File::Path parent    = path.parent_path();
        File::Path extension = path.extension();
       
        if (false == std::filesystem::exists(path))
        {
            return path;
        }
        else
        {
            File::Path newPath = path;
            std::string tail;
            for (int index = 2; index <= maxIndex; ++index)
            {
                tail = " (" + std::to_string(index) + ")";
                newPath = parent / (name + tail + extension);
                if (false == std::filesystem::exists(newPath))
                {
                    break;
                }
            }
            return newPath;
        }
    }
}
