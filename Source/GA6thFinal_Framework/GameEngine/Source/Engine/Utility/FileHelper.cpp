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
        if (fs::exists(path))
        {
            File::Path AbsPath = fs::absolute(path);

            HINSTANCE hr = ShellExecuteW(NULL,            // 부모 윈도우 핸들
                                         L"open",         // 작업(“open”, “edit”, “print” 등)
                                         AbsPath.c_str(), // 실행할 파일 경로
                                         NULL,            // 커맨드라인 인자
                                         NULL,            // 기본 디렉터리
                                         SW_SHOWNORMAL    // 창 표시 방식
            );
            if (reinterpret_cast<UINT_PTR>(hr) <= 32)
            {
                int debugLevel = UmFileSystem.GetDebugLevel();
                if (debugLevel > 0)
                {
                    OutputLog(L"Failed Open File: (" + path.wstring() + L')');
                }
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
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

    bool CopyStrToClipBoard(std::string_view str)
    {
        // 클립보드 열고 비우기
        if (TRUE == OpenClipboard(nullptr))
        {
            if (TRUE == EmptyClipboard())
            {
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
                memcpy(pData, str.data(), size);
                GlobalUnlock(hGlob);

                // CF_TEXT는 ANSI지만, UTF-8도 보통 잘 작동함
                SetClipboardData(CF_TEXT, hGlob);

                CloseClipboard();
                return true;
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
            File::Path  newPath = path;
            std::string tail;
            for (int index = 2; index <= maxIndex; ++index)
            {
                tail    = " (" + std::to_string(index) + ")";
                newPath = parent / (name + tail + extension);
                if (false == fs::exists(newPath))
                {
                    break;
                }
            }
            return newPath;
        }
    }

    bool File::ShowOpenFileDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir,
                                  std::vector<std::pair<LPCWSTR, LPCWSTR>> filters, OUT std::vector<File::Path>& out)
    {
        bool result = ShowOpenFileDialog(owner, title, initialDir, filters, true, out);
        return result;
    }

    bool File::ShowOpenFileDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir,
                                  std::vector<std::pair<LPCWSTR, LPCWSTR>> filters,
                                  OUT File::Path& out)
    {
        std::vector<File::Path> outPath;
        bool result = ShowOpenFileDialog(owner, title, initialDir, filters, false, outPath);
        if (true == result)
        {
            out = outPath.front();
        }
        return result;
    }

    bool File::ShowOpenFileDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir,
                                  std::vector<std::pair<LPCWSTR, LPCWSTR>> filters, bool allowMultiSelect,
                                  OUT std::vector<File::Path>& out)
    {
        DWORD flags = DIRECTORY_DIALOG_FLAG_OPEN_FILE;
        if (allowMultiSelect)
        {
            flags |= DIRECTORY_DIALOG_FLAG_ALLOW_MULTISELECT;
        }

        FileDialogDesc desc;
        desc.Owner            = owner;
        desc.Title            = title;
        desc.InitialDirectory = initialDir;
        desc.DefaultFileName  = L""; // 초기 파일 이름은 없음
        desc.Filters          = filters;
        desc.Flags            = flags;

        return ShowFileDialogEx(desc, out);
    }

    bool File::ShowSaveFileDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir, LPCWSTR defaultName,
                                  const std::vector<std::pair<LPCWSTR, LPCWSTR>>& filters, OUT File::Path& out)
    {
        std::vector<File::Path> outPath;
        DWORD                   flags = DIRECTORY_DIALOG_FLAG_SAVE_FILE;

        FileDialogDesc desc;
        desc.Owner            = owner;
        desc.Title            = title;
        desc.InitialDirectory = initialDir;
        desc.DefaultFileName  = defaultName;
        desc.Filters          = filters;
        desc.Flags            = flags;

        bool result = ShowFileDialogEx(desc, outPath);
        if (true == result)
        {
            out = outPath.front();
        }
        return result;
    }

    bool File::ShowOpenFolderDialog(HWND owner, LPCWSTR title, LPCWSTR initialDir, OUT File::Path& out)
    {
        std::vector<File::Path> outPath;
        DWORD                   flags = DIRECTORY_DIALOG_FLAG_OPEN_FILE | DIRECTORY_DIALOG_FLAG_PICK_FOLDER;

        FileDialogDesc desc;
        desc.Owner            = owner;
        desc.Title            = title;
        desc.InitialDirectory = initialDir;
        desc.DefaultFileName  = L""; // 초기 파일 이름은 없음
        desc.Filters          = {};
        desc.Flags            = flags;

        bool result = ShowFileDialogEx(desc, outPath);
        if (true == result)
            out = outPath.front();
        return result;
    }

    bool ShowFileDialogEx(IN const FileDialogDesc& desc, OUT std::vector<File::Path>& out)
    {
        bool        result = false;
        std::thread tr([&]() {
            // COM 초기화
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            if (FAILED(hr))
            {
                return false;
            }

            IFileDialog* pDialog = nullptr;

            // === 다이얼로그 생성 ===
            if (desc.Flags & DIRECTORY_DIALOG_FLAG_SAVE_FILE)
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
            pDialog->SetTitle(desc.Title);

            // === 옵션 설정 ===
            DWORD options = 0;
            pDialog->GetOptions(&options);
            options |= FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR | FOS_PATHMUSTEXIST; // 기본
            if (desc.Flags & DIRECTORY_DIALOG_FLAG_PICK_FOLDER)
            {
                options |= FOS_PICKFOLDERS; // 폴더 선택
            }
            if (desc.Flags & DIRECTORY_DIALOG_FLAG_ALLOW_MULTISELECT)
            {
                options |= FOS_ALLOWMULTISELECT; // Ctrl/Shift로 다중 파일 선택 허용
            }
            if (desc.Flags & DIRECTORY_DIALOG_FLAG_SAVE_FILE)
            {
                options |= FOS_OVERWRITEPROMPT; // 저장 시 덮어쓰기 경고 (SaveDialog 전용)
            }
            pDialog->SetOptions(options); // 옵션 설정

            // === 필터 설정 ===
            if (false == desc.Filters.empty())
            {
                std::vector<COMDLG_FILTERSPEC> specs;
                specs.reserve(desc.Filters.size());
                for (const auto& [name, spec] : desc.Filters)
                {
                    specs.push_back({name, spec});
                }
                pDialog->SetFileTypes(static_cast<UINT>(specs.size()), specs.data());
                pDialog->SetFileTypeIndex(1); // 첫 번째 필터 선택
            }

            // === 기본 폴더 설정 ===
            fs::path defaultAbsPath = desc.InitialDirectory;
            if (true == defaultAbsPath.empty())
            {
                defaultAbsPath = UmFileSystem.GetRootPath();
            }
            defaultAbsPath         = fs::absolute(defaultAbsPath);
            IShellItem* folderItem = nullptr;
            hr = SHCreateItemFromParsingName(defaultAbsPath.c_str(), nullptr, IID_PPV_ARGS(&folderItem));
            if (SUCCEEDED(hr))
            {
                pDialog->SetFolder(folderItem); // 기본 폴더 설정
                folderItem->Release();
            }

            // === 기본 파일 이름 설정 ===
            if (desc.Flags & DIRECTORY_DIALOG_FLAG_SAVE_FILE)
            {
                // 저장 플래그에만 적용
                pDialog->SetFileName(desc.DefaultFileName);
            }

            // 다이얼로그 실행
            bool isGetPath = false;
            hr             = pDialog->Show(NULL);
            if (SUCCEEDED(hr))
            {
                if ((desc.Flags & DIRECTORY_DIALOG_FLAG_ALLOW_MULTISELECT) &&
                    (desc.Flags & DIRECTORY_DIALOG_FLAG_OPEN_FILE))
                {
                    IFileOpenDialog* openDlg = nullptr;
                    hr                       = pDialog->QueryInterface(IID_PPV_ARGS(&openDlg));
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
                    hr                = pDialog->GetResult(&pItem);
                    if (SUCCEEDED(hr) && pItem)
                    {
                        LPWSTR pszFilePath = nullptr;
                        hr                 = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
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
            result = isGetPath;
            return isGetPath;
        });
        tr.join();
        return result;
    }

    std::wstring_view GetDesktopPath()
    {
        static std::wstring desktopPath;
        if (true == desktopPath.empty())
        {
            PWSTR   pszPath = NULL; // 경로를 저장할 와이드 문자열 포인터
            HRESULT hr      = SHGetKnownFolderPath(FOLDERID_Desktop, 0, NULL, &pszPath);
            if (SUCCEEDED(hr))
            {
                desktopPath = pszPath;
            }
            else
            {
                desktopPath = L"C:";
            }

            if (pszPath)
            {
                CoTaskMemFree(pszPath);
            }
        }
        return desktopPath;
    }

    void SetClipboardText(std::wstring_view text) 
    {
        // 1. 클립보드를 엽니다.
        if (!OpenClipboard(nullptr))
        {
            // 클립보드를 열 수 없으면 실패
            return;
        }

        // 2. 클립보드의 기존 내용을 비웁니다.
        EmptyClipboard();

        // 3. 문자열을 저장할 전역 메모리를 할당합니다.
        // GMEM_MOVEABLE은 메모리를 필요에 따라 이동하거나 확장할 수 있게 합니다.
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, (text.length() + 1) * sizeof(wchar_t));
        if (hGlobal == NULL)
        {
            CloseClipboard();
            return;
        }

        // 4. 메모리 핸들을 잠그고 실제 포인터를 얻습니다.
        LPWSTR pGlobal = static_cast<LPWSTR>(GlobalLock(hGlobal));
        if (pGlobal == NULL)
        {
            GlobalFree(hGlobal);
            CloseClipboard();
            return;
        }

        // 5. 할당된 메모리에 문자열 데이터를 복사합니다.
        memcpy(pGlobal, text.data(), (text.length() + 1) * sizeof(wchar_t));

        // 6. 메모리 잠금을 해제합니다.
        GlobalUnlock(hGlobal);

        // 7. 클립보드에 데이터를 설정합니다. 이제부터 이 메모리는 시스템 소유입니다.
        // 우리가 직접 GlobalFree를 호출하면 안 됩니다.
        if (SetClipboardData(CF_UNICODETEXT, hGlobal) == NULL)
        {
            // 실패했다면 시스템이 소유권을 가져가지 않은 것이므로 우리가 직접 해제해야 합니다.
            GlobalFree(hGlobal);
        }

        // 8. 클립보드를 닫습니다.
        CloseClipboard();
    }

    std::wstring GetClipboardText()
    {
        // 1. 클립보드에 유니코드 텍스트 형식이 있는지 확인합니다.
        if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
        {
            return L""; // 없으면 빈 문자열 반환
        }

        // 2. 클립보드를 엽니다.
        if (!OpenClipboard(nullptr))
        {
            return L""; // 열 수 없으면 실패
        }

        // 3. 클립보드에서 데이터 핸들을 가져옵니다.
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData == NULL)
        {
            CloseClipboard();
            return L"";
        }

        // 4. 메모리 핸들을 잠그고 읽기 전용 포인터를 얻습니다.
        const wchar_t* pszData = static_cast<const wchar_t*>(GlobalLock(hData));
        if (pszData == NULL)
        {
            CloseClipboard();
            return L"";
        }

        // 5. 데이터를 std::wstring으로 복사합니다.
        std::wstring text(pszData);

        // 6. 메모리 잠금을 해제합니다.
        GlobalUnlock(hData);

        // 7. 클립보드를 닫습니다.
        CloseClipboard();

        return text;
    }
    std::time_t GetFileLastWriteTime(const fs::directory_entry& entry)
    {
        auto ftime = fs::last_write_time(entry);
        auto sctp  = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
        return cftime;
    }
    bool Compare::CompareByType(const fs::directory_entry& a, const fs::directory_entry& b) const
    {
        bool aIsDir = a.is_directory();
        bool bIsDir = b.is_directory();
        return aIsDir > bIsDir;
    }

    bool Compare::CompareByName(const fs::directory_entry& a, const fs::directory_entry& b) const
    {
        std::string aName = a.path().filename().string();
        std::string bName = b.path().filename().string();
        return aName < bName;
    }

    bool Compare::CompareByDate(const fs::directory_entry& a, const fs::directory_entry& b) const
    {
        auto timeA = fs::last_write_time(a);
        auto timeB = fs::last_write_time(b);
        return timeA > timeB; // 최신순
    }
} // namespace File