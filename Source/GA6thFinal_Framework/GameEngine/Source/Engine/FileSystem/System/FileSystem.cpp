#include "pch.h"
#include "FileSystem.h"

using namespace File;
using namespace u8_literals;

bool EFileSystem::CreateProject(const File::Path& path)
{
    bool isExists = fs::exists(path);
    bool isValid  = path.extension() == PROJECT_EXTENSION;

    if (true == isExists)
    {
        OutputLog(L"failed to EFileSystem::CreateProject. already exists to project.");
        return false;
    }

    if (false == isValid)
    {
        OutputLog(L"failed to EFileSystem::CreateProject. file is unvaild extension.");
        return false;
    }

    if (false == _projectData.Create(path, false))
    {
        OutputLog(L"failed to EFileSystem::CreateProject. failed to create project file.");
        return false;
    }

    File::Path rootPath     = path.parent_path();
    File::Path assetPath    = rootPath / ASSET_FOLDER_NAME;
    File::Path settingPath  = rootPath / PROJECT_SETTING_PATH;
    File::CreateFolder(assetPath.generic_wstring());
    File::CreateFolder(settingPath.generic_wstring());

    return true;
}

bool EFileSystem::LoadProject(const File::Path& path)
{
    bool isExists = fs::exists(path);
    bool isValid  = path.extension() == PROJECT_EXTENSION;

    std::pair<bool, std::wstring> result = {true, L""};
    if (true == result.first && false == isExists)
    {
        result.second = L"존재하지 않는 경로입니다.";
        result.first  = false;
    }
    if (true == result.first && false == isValid)
    {
        result.second = L"프로젝트 확장자가 올바르지 않습니다.";
        result.first  = false;
    }

    if (true == result.first && false == _projectData.Load(path))
    {
        result.second = L"프로젝트 파일 로드에 실패하였습니다.";
        result.first  = false;
    }

    if (false == result.first)
    {
        std::wstring msg    = result.second;
        std::wstring title  = L"Error";
        int result = MessageBox(
            GetFocus(),                 // 부모 창 핸들 (NULL로 하면 독립적 메시지 박스)
            msg.c_str(),                // 메시지 텍스트
            title.c_str(),              // 메시지 박스 제목
            MB_OK | MB_ICONWARNING      // 스타일: 예/아니오 버튼 + 질문 아이콘
        );

        return false;
    }

    File::Path directory = path.parent_path();

    _projectName = path.stem().string();
    _originPath  = fs::current_path().generic_wstring();
    _rootPath    = fs::absolute(directory).generic_wstring();
    _assetPath   = fs::absolute(_rootPath / ASSET_FOLDER_NAME).generic_wstring();
    _settingPath = fs::absolute(_rootPath / PROJECT_SETTING_PATH).generic_wstring();

    File::CreateFolder(_assetPath);
    File::CreateFolder(_settingPath);

    if (nullptr != _observer)
    {
        _observer->Stop();
        _observer->SetObservingPath(_rootPath);
        _observer->Start();
    }

    for (auto& notifier : _notifierSet)
        notifier->OnRequestedLoad();

    ReadDirectory();

    for (auto& notifier : _notifierSet)
        notifier->OnPostRequestedLoad();

    return true;
}

bool EFileSystem::SaveProject()
{
    if (true == _rootPath.empty())
        return false;

    if (false == fs::exists(_rootPath))
    {
        return false;
    }

    for (auto& notifier : _notifierSet)
        notifier->OnRequestedSave();

    for (auto& notifier : _notifierSet)
        notifier->OnPostRequestedSave();

    return true;
}

bool EFileSystem::SaveAsProject(const File::Path& to)
{
    if (true == _projectData.IsNull())
        return false;

    std::wstring msg    = L"현재 프로젝트를 저장하고 다른 이름으로 저장합니다.";
    std::wstring title  = L"Save As Project";

    int result = MessageBox(
        GetFocus(),               // 부모 창 핸들 (NULL로 하면 독립적 메시지 박스)
        msg.c_str(),              // 메시지 텍스트
        title.c_str(),            // 메시지 박스 제목
        MB_YESNO                  // 스타일: 예/아니오 버튼
    );

    if (result == IDYES)
    {
        SaveProject();
        fs::copy(_rootPath, to, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        return true;
    }
    else
    {
        return false;
    }
}

bool EFileSystem::LoadProjectWithMessageBox(const File::Path& path)
{
    File::Path projectName = path.filename();

    std::wstring msg    = projectName.wstring() + L" 프로젝트를 로드하시겠습니까?";
    std::wstring title  = L"Load Project";
    int result = MessageBox(
        GetFocus(),                 // 부모 창 핸들 (NULL로 하면 독립적 메시지 박스)
        msg.c_str(),                // 메시지 텍스트
        title.c_str(),              // 메시지 박스 제목
        MB_YESNO                    // 스타일: 예/아니오 버튼
    );

    if (result == IDYES)
    {
        return LoadProject(path);
    }

    return false;
}

bool EFileSystem::SaveProjectWithMessageBox()
{
    if (true == _rootPath.empty())
        return false;

    std::wstring msg    = L"현재 프로젝트를 저장하시겠습니까?"; 
    std::wstring title  = L"Save Project";

    HWND hwnd = UmApplication.GetHwnd();

    int result = MessageBox(
        hwnd,                       // 부모 창 핸들 (NULL로 하면 독립적 메시지 박스)
        msg.c_str(),                // 메시지 텍스트
        title.c_str(),              // 메시지 박스 제목
        MB_YESNO                    // 스타일: 예/아니오 버튼
    );

    if (result == IDYES)
    {
        return SaveProject();
    }
    return false;
}

bool EFileSystem::SaveSetting(const File::Path& path)
{
    auto setting = rfl::yaml::save(path.string(), _setting);
    if (false == setting)
    {
        return false;
    }
    else
    {
        return LoadSetting(path);
    }
}

bool EFileSystem::LoadSetting(const File::Path& path) 
{
    auto setting = rfl::yaml::load<File::SystemSetting>(path.string());
    if (false == setting)
    {
        return false;
    }
    else
    {
        _setting = setting.value();
        return true;
    }
}

void EFileSystem::ObserverSetUp(const CallBackFunc& callback)
{
    if (nullptr == _observer)
    {
        _observer = new File::FileObserver();
        _observer->SetCallbackFunc(callback);
        _observer->SetObservingPath(_rootPath);
        _observer->Start();
    }
}

void EFileSystem::ObserverShutDown() 
{
    if (nullptr != _observer)
    {
        _observer->Stop();
        delete _observer;
        _observer = nullptr;
    }
}

bool EFileSystem::IsVaildGuid(const File::Guid& guid) const
{
    return _guidToPathTable.find(guid) != _guidToPathTable.end();
}

bool EFileSystem::IsValidExtension(const File::FString& ext) const
{
    return (ext == "") || (_extesionToNotifierTable.find(ext) != _extesionToNotifierTable.end());
}

bool EFileSystem::IsSameContext(std::weak_ptr<File::Context> left, std::weak_ptr<File::Context> right) const
{
    if (false == left.expired() && false == right.expired())
    {
        auto spLeft  = left.lock();
        auto spRight = right.lock();
        if (spLeft == spRight)
        {
            return true;
        }
    }
    return false;
}

File::Path EFileSystem::GetRelativePath(const File::Path& path) const
{
    if (false == _projectData.IsNull())
    {
        File::Path out = fs::absolute(path);
        out = fs::relative(out, _rootPath);
        return out;
    }
    return File::NULL_PATH;
}

const File::Path& EFileSystem::GetPathFromGuid(const File::Guid& guid) const
{
    auto wpContext = GetContext(guid);
    if (false == wpContext.expired())
    {
        auto& path = wpContext.lock()->GetPath();
        return path;
    }
    return NULL_PATH;
}
const File::Guid& EFileSystem::GetGuidFromPath(const File::Path& path) const
{
    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        const MetaData& meta = wpContext.lock()->GetMeta();
        return meta.GetGuid();
    }
    return NULL_GUID;
}
std::weak_ptr<Context> EFileSystem::GetContext(const File::Guid& guid) const
{
    if (NULL_GUID == guid)
    {
        return std::weak_ptr<Context>();
    }

    auto itr = _guidToPathTable.find(guid);
    if (itr != _guidToPathTable.end())
    {
        return itr->second; 
    }
    else
    {
        return std::weak_ptr<Context>();
    }
    
}
std::weak_ptr<Context> EFileSystem::GetContext(const File::Path& path) const
{
    auto itr = _pathToGuidTable.find(path);
    if (itr != _pathToGuidTable.end())
    {
        return itr->second; 
    }
    else
    {
        return std::weak_ptr<Context>();
    }
}

std::unordered_set<File::FileEventNotifier*> EFileSystem::GetNotifiers(
    const File::FString& ext)
{
    auto itr = _extesionToNotifierTable.find(ext);
    if (itr != _extesionToNotifierTable.end())
    {
        return itr->second;
    }
    return std::unordered_set<File::FileEventNotifier*>();
}

void EFileSystem::RequestInspectFile(const File::Path& path)
{
    NotifierSet notifierSet = GetNotifiers(path.extension());
    for (auto& notifier : notifierSet)
    {
        notifier->OnRequestedInspect(path);
    }
}

void EFileSystem::RequestOpenFile(const File::Path& path) 
{
    NotifierSet notifierSet = GetNotifiers(path.extension());
    for (auto& notifier : notifierSet)
    {
        notifier->OnRequestedOpen(path);
    }
}

void EFileSystem::RequestCopyFile(const File::Path& path) 
{
    NotifierSet notifierSet = GetNotifiers(path.extension());
    for (auto& notifier : notifierSet)
    {
        notifier->OnRequestedCopy(path);
    }
}

void EFileSystem::RequestPasteFile(const File::Path& path) 
{
    NotifierSet notifierSet = GetNotifiers(path.extension());
    for (auto& notifier : notifierSet)
    {
        notifier->OnRequestedPaste(path);
    }
}

void EFileSystem::DrawGuiSettingEditor() 
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                HWND    owner = UmApplication.GetHwnd();
                LPCWSTR title = L"폴더를 선택하세요.";

                File::Path directory = _rootPath / PROJECT_SETTING_PATH;
                directory            = directory.generic_wstring();
                if (File::ShowOpenFolderBrowser(owner, title, _rootPath.c_str(), directory))
                {
                    File::Path filename  = L"FileSystem.UmSetting";
                    SaveSetting(directory / filename);
                }
            }
            if (ImGui::MenuItem("Load"))
            {
                HWND       owner   = UmApplication.GetHwnd();
                LPCWSTR    title = L"폴더를 선택하세요.";
                File::Path path;
                if (File::ShowOpenFolderBrowser(owner, title, _rootPath.c_str(), path))
                {
                    File::Path filename  = L"fileSystem.setting";
                    File::Path directory = PROJECT_SETTING_PATH;
                    SaveSetting(directory / filename);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("Debug Level: ");
    ImGui::DragInt("##DragDebuglevel", &_setting.DebugLevel, 0, 3);

    static char metaExt[128] = "";
    strcpy_s(metaExt, _setting.MetaExt.c_str());
    ImGui::Text("Meta Extension: ");
    if (ImGui::InputText("##InputMetaExt", metaExt, IM_ARRAYSIZE(metaExt)))
    {
        _setting.MetaExt = metaExt;
    }
}


void EFileSystem::RegisterFileEventNotifier(FileEventNotifier* notifier, const std::initializer_list<const char*>& exts)
{
    if (notifier == nullptr)
        return;

    for (const auto& ext : exts)
    {
        notifier->_triggerExtTable.insert(ext);
        _extesionToNotifierTable[ext].insert(notifier);
    }
    auto itr = _notifierSet.find(notifier);
    if (itr == _notifierSet.end())
    {
        _notifierSet.insert(notifier);
    }
}

void EFileSystem::UnRegisterFileEventNotifier(FileEventNotifier* notifier) 
{
    const std::vector<FString> exts = notifier->GetTriggerExtensions();

    for (const auto& ext : exts)
    {
        auto itr = _extesionToNotifierTable.find(ext);
        if (itr != _extesionToNotifierTable.end())
        {
            auto& notifierSet = itr->second;
            notifierSet.erase(notifier);
            if (notifierSet.empty())
            {
                _extesionToNotifierTable.erase(itr);
            }
        }
    }
    auto itr = _notifierSet.find(notifier);
    if (itr != _notifierSet.end())
    {
        _notifierSet.erase(itr);
    }
}

void EFileSystem::Clear()
{        
    ClearContext();
    ClearNotifier();
}

void EFileSystem::ClearContext()
{
    
    for (auto& context : _contextTable)
    {
        if (nullptr != context.get())
        {
            auto& path = context->GetPath();
            File::Path  extension   = path.extension();
            NotifierSet notifierSet = GetNotifiers(extension);
            for (auto& notifier : notifierSet)
            {
                notifier->OnFileUnregistered(path);
            }
        }
    }
    _pathToGuidTable.clear();
    _guidToPathTable.clear();
    _contextTable.clear();
}

void EFileSystem::ClearNotifier() 
{
    _extesionToNotifierTable.clear();
}

void EFileSystem::ReadDirectory() 
{
    ClearContext();
    ReadDirectory(_rootPath);
}

void EFileSystem::ReadDirectory(const File::Path& path) 
{
    File::Path extesion = path.extension();
    File::Path genPath  = path.generic_string();

    if (true == IsValidExtension(extesion))
    {
        RegisterContext(genPath);
    }
    if (true == fs::is_directory(genPath))
    {
        for (const auto& entry : fs::recursive_directory_iterator(genPath))
        {
            // 경로를 재네릭화
            File::Path genericPath = entry.path().generic_string();
            ReadDirectory(genericPath);
        }
    }
}

void EFileSystem::RegisterContext(const File::Path& path) 
{
    // 파일이 없으면 return
    if (false == fs::exists(path))
        return;

    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(path.extension()))
        return;

     auto find = GetContext(path);

     if (false != find.expired())
     {
         std::shared_ptr<Context> context;

         auto absPath = fs::weakly_canonical(path);
         absPath      = absPath.generic_wstring();
         if (true == fs::is_regular_file(absPath))
         {
             context = std::make_shared<FileContext>(absPath);
         }
         else if (true == fs::is_directory(absPath))
         {
             context = std::make_shared<FolderContext>(absPath);
         }
         else
         {
             return;
         }

         // 부모 폴더에서 자신을 추가한다.
         File::Path parentPath = absPath.parent_path().generic_string();
         if (parentPath == "")  // 비어있으면 현재 디렉터리임
             parentPath = ".";

         auto parentContext = UmFileSystem.GetContext<FolderContext>(parentPath);
         if (false == parentContext.expired())
         {
             auto spParentContext = parentContext.lock();
             File::Path filename  = absPath.filename();
             spParentContext->_contextTable[filename] = context;
         }

         auto& meta = context->GetMeta();
         auto& guid = meta.GetGuid();

         _pathToGuidTable[absPath] = context;
         _guidToPathTable[guid] = context;
         _contextTable.insert(context);

         context->OnFileRegistered(absPath);

         File::Path  extension   = absPath.extension();
         NotifierSet notifierSet = GetNotifiers(extension);
         for (auto& notifier : notifierSet)
         {
             notifier->OnFileRegistered(absPath);
         }
     }
}

void EFileSystem::UnregisterContext(const File::Path& path) 
{
    // 이미 삭제된 파일이므로 존재 검사를 하면 안된다...
   
    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(path.extension()))
        return;

    auto wpContext = GetContext(path);

    if (false == wpContext.expired())
    {
        auto spContext  = wpContext.lock();
        auto& meta      = spContext->GetMeta();
        auto& guid      = meta.GetGuid();

        File::Path  extension   = path.extension();
        NotifierSet notifierSet = GetNotifiers(extension);
        for (auto& notifier : notifierSet)
        {
            notifier->OnFileUnregistered(path);
        }

        _contextTable.erase(spContext);
        _pathToGuidTable.erase(path);
        _guidToPathTable.erase(guid);
    }
}

void EFileSystem::ProcessRemovedFile(const File::Path& path)
{
    if (path.extension() == UmFileSystem.GetMetaExt())
    {
        // 메타 파일을 메모리에 존재하는 guid로 재생성
        File::Path filePath = path;
        filePath.replace_extension("");
        // 원본 파일이 존재할 때 만
        if (true == fs::exists(filePath))
        {
            auto wpContext = GetContext(filePath);
            if (false == wpContext.expired())
            {
                auto  spContext = wpContext.lock();
                auto& meta      = spContext->GetMeta();
                meta.FileCreate();
            }
        }
        return;
    }

    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(path.extension()))
        return;

    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        auto  spContext = wpContext.lock();
        auto& meta      = spContext->GetMeta();
        auto& guid      = meta.GetGuid();

        auto notifierSet = GetNotifiers(path.extension());
        for (auto& notifier : notifierSet)
        {
            notifier->OnFileRemoved(path);
        }
        UmFileSystem.UnregisterContext(path);

        spContext->OnFileRemoved(path);

        // 부모 폴더에서 자신을 제거한다.
        File::Path parentPath = path.parent_path().generic_string();
        auto wpFolderContext = EFileSystem::GetContext<FolderContext>(parentPath);
        if (false == wpFolderContext.expired())
        {
            wpFolderContext.lock()->_contextTable.erase(path.filename());
        }
    }
}

void EFileSystem::ProcessModifiedFile(const File::Path& path)
{
    // 파일이 없으면 return
    if (false == fs::exists(path))
        return;

    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(path.extension()))
        return;

    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        auto spContext = wpContext.lock();

        spContext->OnFileModified(path);

        auto notifierSet = GetNotifiers(path.extension());
        for (auto& notifier : notifierSet)
        {
            notifier->OnFileModified(path);
        } 
    }
}

void EFileSystem::ProcessMovedFile(const File::Path& oldPath, const File::Path& newPath) 
{
    // 파일이 없으면 return
    if (false == fs::exists(newPath))
        return;

    // 확장자가 유효하지 않으면 return
    if (false == IsValidExtension(newPath.extension()))
        return;

    // 이전 경로에서 컨텍스트를 찾아 새 경로로 옮기기
    auto wpContext = GetContext(oldPath);
    if (false == wpContext.expired())
    {
        auto spContext = wpContext.lock();

        File::Path oldFileName = oldPath.filename();
        File::Path oldFolderPath = oldPath.parent_path().generic_string();
        File::Path oldExtension  = oldPath.extension();
        File::Path newFileName = newPath.filename();
        File::Path newFolderPath = newPath.parent_path().generic_string();
        File::Path newExtension  = newPath.extension();

        _pathToGuidTable.erase(oldPath);
        _pathToGuidTable[newPath] = spContext;
        // Guid는 동일하므로 안지워도 된다.

        if (oldFolderPath == newFolderPath)
        {   // 같은 폴더 내에서 이름만 변경
            spContext->OnFileRenamed(oldPath, newPath);
        }
        else
        {   // 다른 폴더로 이동
            spContext->OnFileMoved(oldPath, newPath);
        }

        auto oldFolderContext =
            UmFileSystem.GetContext<FolderContext>(oldFolderPath);
        auto newFolderContext =
            UmFileSystem.GetContext<FolderContext>(newFolderPath);

        if (false == oldFolderContext.expired() && false == newFolderContext.expired())
        {
            FString oldFileName = oldPath.filename();
            FString newFileName = newPath.filename();
            oldFolderContext.lock()->_contextTable.erase(oldFileName);
            newFolderContext.lock()->_contextTable[newFileName] = spContext;
        }

        if (oldFolderPath == newFolderPath)
        {
            auto notifierSet = GetNotifiers(newExtension);
            for (auto& notifier : notifierSet)
            {
                notifier->OnFileRenamed(oldPath, newPath);
            } 
        }
        else
        {
            auto notifierSet = GetNotifiers(newExtension);
            for (auto& notifier : notifierSet)
            {
                notifier->OnFileMoved(oldPath, newPath);
            } 
        }
    }
}