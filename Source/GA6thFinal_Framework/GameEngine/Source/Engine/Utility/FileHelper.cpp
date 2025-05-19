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

    bool ShowOpenFileBrowser(HWND owner, LPCWSTR title, LPCWSTR initialDir,
                             std::vector<std::pair<LPCWSTR, LPCWSTR>> filters, bool allowMultiSelect,
                             OUT std::vector<File::Path>& out)
    {
        DirectoryBrowserFlags flags = DIRECTORY_BROWSER_FLAG_OPEN_FILE;
        if (allowMultiSelect)
        {
            flags |= DIRECTORY_BROWSER_FLAG_ALLOW_MULTISELECT;
        }
        return ShowSelectDirectoryBrowserEx(owner, title, initialDir, L"", filters, flags, out);
    }

    bool ShowSaveFileBrowser(HWND owner, LPCWSTR title, LPCWSTR initialDir, LPCWSTR defaultName, OUT File::Path& out)
    {
        std::vector<std::pair<LPCWSTR, LPCWSTR>> filters;
        std::vector<File::Path>                  outPath;
        DirectoryBrowserFlags                    flags = DIRECTORY_BROWSER_FLAG_SAVE_FILE;

        bool result = ShowSelectDirectoryBrowserEx(owner, title, initialDir, defaultName, filters, flags, outPath);
        if (true == result)
            out = outPath.front();
        return result;
    }

    bool ShowOpenFolderBrowser(HWND owner, LPCWSTR title, LPCWSTR initialDir, OUT File::Path& out)
    {
        std::vector<std::pair<LPCWSTR, LPCWSTR>> filters;
        std::vector<File::Path>                  outPath;
        DirectoryBrowserFlags flags = DIRECTORY_BROWSER_FLAG_OPEN_FILE | DIRECTORY_BROWSER_FLAG_PICK_FOLDER;

        bool result = ShowSelectDirectoryBrowserEx(owner, title, initialDir, L"", filters, flags, outPath);
        if (true == result)
            out = outPath.front();
        return result;
    }

    bool ShowSelectDirectoryBrowserEx(HWND owner, LPCWSTR title, LPCWSTR initialDirectory, LPCWSTR initialFileName,
                                                           std::vector<std::pair<LPCWSTR, LPCWSTR>> filters,
                                                           DirectoryBrowserFlags flags, 
                                                           OUT std::vector<File::Path>& out)
    {
        // COM 초기화
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr))
        {
            return false;
        }

        IFileDialog* pDialog = nullptr;

        // === 다이얼로그 생성 ===
        if (flags & DIRECTORY_BROWSER_FLAG_SAVE_FILE)
        {
            hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDialog));
        }
        else
        {
            hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDialog));
        }
        if (FAILED(hr))
        {
            CoUninitialize();
            return false;
        }

        // === 타이틀 설정 ===
        pDialog->SetTitle(title);

        // === 옵션 설정 ===
        DWORD options = 0;
        pDialog->GetOptions(&options);
        options |= FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST; // 기본
        if (flags & DIRECTORY_BROWSER_FLAG_PICK_FOLDER)
        {
            options |= FOS_PICKFOLDERS; // 폴더 선택
        }
        if (flags & DIRECTORY_BROWSER_FLAG_ALLOW_MULTISELECT)
        {
            options |= FOS_ALLOWMULTISELECT; // Ctrl/Shift로 다중 파일 선택 허용
        }
        if (flags & DIRECTORY_BROWSER_FLAG_SAVE_FILE)
        {
            options |= FOS_OVERWRITEPROMPT; // 저장 시 덮어쓰기 경고 (SaveDialog 전용)
        }
        pDialog->SetOptions(options); // 옵션 설정

        // === 필터 설정 ===
        if (false == filters.empty())
        {
            std::vector<COMDLG_FILTERSPEC> specs;
            specs.reserve(filters.size());
            for (auto& [name, spec] : filters)
            {
                specs.push_back({name, spec});
            }
            pDialog->SetFileTypes(static_cast<UINT>(specs.size()), specs.data());
            pDialog->SetFileTypeIndex(1); // 첫 번째 필터 선택
        }

        // === 기본 폴더 설정 ===
        fs::path defaultAbsPath = initialDirectory;
        if (true == defaultAbsPath.empty())
        {
            defaultAbsPath = fs::current_path();
        }
        defaultAbsPath = fs::absolute(defaultAbsPath);
        IShellItem* folderItem = nullptr;
        hr = SHCreateItemFromParsingName(defaultAbsPath.c_str(), nullptr, IID_PPV_ARGS(&folderItem));
        if (SUCCEEDED(hr))
        {
            pDialog->SetFolder(folderItem); // 기본 폴더 설정
            folderItem->Release();
        }

        // === 기본 파일 이름 설정 ===
        if (flags & DIRECTORY_BROWSER_FLAG_SAVE_FILE)
        {
            // 저장 플래그에만 적용
            pDialog->SetFileName(initialFileName);
        }

        // 다이얼로그 실행
        bool isGetPath = false;
        hr = pDialog->Show(owner);
        if (SUCCEEDED(hr))
        {
            if ((flags & DIRECTORY_BROWSER_FLAG_ALLOW_MULTISELECT) && (flags & DIRECTORY_BROWSER_FLAG_OPEN_FILE))
            {
                IFileOpenDialog* openDlg = nullptr;
                hr = pDialog->QueryInterface(IID_PPV_ARGS(&openDlg));
                if (SUCCEEDED(hr))
                {
                    IShellItemArray* pItems = nullptr;
                    hr                      = openDlg->GetResults(&pItems);
                    if (SUCCEEDED(hr) && nullptr != pItems)
                    {
                        DWORD count = 0;
                        pItems->GetCount(&count);
                        for (DWORD i = 0; i < count; ++i)
                        {
                            IShellItem* pItem = nullptr;
                            hr                = pItems->GetItemAt(i, &pItem);
                            if (SUCCEEDED(hr))
                            {
                                PWSTR pszFilePath = nullptr;
                                hr                = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                                if (SUCCEEDED(hr))
                                {
                                    out.emplace_back(pszFilePath);
                                    CoTaskMemFree(pszFilePath);
                                    isGetPath = true;
                                }
                                pItem->Release();
                            }
                        }
                        pItems->Release();
                    }
                }
            }
            else
            {
                IShellItem* pItem = nullptr;
                hr = pDialog->GetResult(&pItem);
                if (SUCCEEDED(hr) && pItem)
                {
                    LPWSTR pszFilePath = nullptr;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr))
                    {
                        out.emplace_back(pszFilePath);
                        CoTaskMemFree(pszFilePath);
                        isGetPath = true;
                    }
                    pItem->Release();
                }
            }
        }
        pDialog->Release();
        CoUninitialize();
        return isGetPath;
    }
} 