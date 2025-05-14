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
        bool isExists    = fs::exists(path);
        bool isDirectory = (false == path.has_extension());
        if (false == isExists && true == isDirectory)
        {
            bool check = fs::create_directory(path);
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
        bool isExists    = fs::exists(path);
        bool isDirectory = (false == path.has_extension());
        if (true == isDirectory)
        {
            if (false == isExists)
            {
                return CreateFolder(path);
            }
            else if (true == processDup)
            {
                return CreateFolder(GenerateUniquePath(path));
            }
        }
        return false;
    }

    bool OpenFile(const File::Path& path)
    {
        File::Path AbsPath = fs::absolute(path);

        HINSTANCE hr = ShellExecuteW(NULL,         // 부모 윈도우 핸들
                                     L"open",      // 작업(“open”, “edit”, “print” 등)
                                     AbsPath.c_str(), // 실행할 파일 경로
                                     NULL,         // 커맨드라인 인자
                                     NULL, // 기본 디렉터리
                                     SW_SHOWNORMAL // 창 표시 방식
        );
        if (reinterpret_cast<UINT_PTR>(hr) <= 32)
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
        if (true == fs::exists(path))
        {
            if (fs::is_directory(path))
            {
                return fs::remove_all(path); // 디렉토리 삭제
            }
            else
            {
                return fs::remove(path); // 파일 삭제
            }
        }
        return false;
    }

    bool CopyFileFromTo(const File::Path& from, File::Path to)
    {
        to = File::GenerateUniquePath(to.generic_string());

        // 디렉터리 복사는 버그때문에 안하기로 함.
        // (폴더 내의 파일은 이벤트가 발생하지 않기 때문)
        if (false == fs::is_directory(from))
        {
            fs::copy_file(from, to);
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
    Path GenerateUniquePath(const File::Path& path, int maxIndex)
    {
        // 중복일 시 뒤에 () 붙여서 생성
        File::Path name      = path.stem();
        File::Path parent    = path.parent_path();
        File::Path extension = path.extension();
       
        if (false == fs::exists(path))
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
                if (false == fs::exists(newPath))
                {
                    break;
                }
            }
            return newPath;
        }
    }

    bool OpenFileNameBrowser(TCHAR* _filter, File::Path& _return, const File::Path& root)
    {
        OPENFILENAME OFN;
        TCHAR        lpstrFile[MAX_PATH]    = L"";

        memset(&OFN, 0, sizeof(OPENFILENAME));

        OFN.lStructSize     = sizeof(OPENFILENAME);
        OFN.hwndOwner       = UmApplication.GetHwnd();
        OFN.lpstrFilter     = _filter;
        OFN.lpstrFile       = lpstrFile;
        OFN.nMaxFile        = MAX_PATH;
        OFN.lpstrInitialDir = root.c_str();
        
        auto originPath = fs::current_path();
        bool result = false;
        if (GetOpenFileName(&OFN))
        {
            _return = OFN.lpstrFile;
            result =  true;
        }
        fs::current_path(originPath);
        return result;
    }

    // 콜백 함수
    static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
    {
        if (uMsg == BFFM_INITIALIZED)
        {
            if (lpData != NULL)
                SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
        }
        return 0;
    }

    bool OpenForderBrowser(TCHAR* title, UINT flags, File::Path& out, const File::Path& root)
    {
        PIDLIST_ABSOLUTE pidlRoot = nullptr;
        SFGAOF           sfgao    = 0;

        File::Path absRoot = fs::absolute(root);
        File::Path absPath = fs::absolute(out);

        if (FAILED(SHParseDisplayName(absRoot.c_str(), NULL, &pidlRoot, 0, &sfgao)))
        {
            pidlRoot = NULL; // 실패하면 NULL 처리 (Desktop 기준으로)
        }

        BROWSEINFO bi = {0};
        bi.hwndOwner  = NULL;
        bi.lpszTitle  = title;
        bi.ulFlags    = flags;
        bi.lpfn       = BrowseCallbackProc;
        bi.pidlRoot   = pidlRoot; // PIDL 설정!

        // 시작 폴더용 lParam 설정
        bi.lParam = reinterpret_cast<LPARAM>(absPath.c_str());

        // 폴더 선택 대화상자 열기
        LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
        if (pidl != NULL)
        {
            wchar_t path[MAX_PATH];
            if (SHGetPathFromIDList(pidl, path))
            {
                out = path;
                CoTaskMemFree(pidl); // pidl 해제
                if (pidlRoot)
                    CoTaskMemFree(pidlRoot); // 루트도 해제
                return true;
            }
            CoTaskMemFree(pidl);
        }
        if (pidlRoot)
            CoTaskMemFree(pidlRoot); // 루트도 해제
        return false;
    }
}
