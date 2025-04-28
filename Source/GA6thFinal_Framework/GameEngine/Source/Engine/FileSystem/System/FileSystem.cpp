#include "pch.h"
#include "FileSystem.h"

using namespace File;

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

    std::ofstream projectof(path);
    if (false == projectof.is_open())
    {
        return false;
    }
    projectof.close();

    fs::path assetPath = path / ASSET_FOLDER_NAME;
    fs::path settingPath = path / PROJECT_SETTING_PATH;
    File::CreateFolder(assetPath);
    File::CreateFolder(settingPath);

    return true;
}

bool EFileSystem::LoadProject(const File::Path& path)
{
    bool isExists = fs::exists(path);
    bool isValid  = path.extension() == PROJECT_EXTENSION;

    if (false == isExists)
    {
        OutputLog(L"failed to EFileSystem::CreateProject. not exists to direcotry.");
        return false;
    }
    if (false == isValid)
    {
        OutputLog(L"failed to EFileSystem::CreateProject. file is unvaild extension.");
        return false;
    }

    File::Path directory = path.parent_path();

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

    LoadSetting(_settingPath / PROJECT_SETTING_FILENAME);
    ReadDirectory(); 

    return true;
}

bool EFileSystem::SaveProject()
{
    if (false == fs::exists(_rootPath))
    {
        return false;
    }

    SaveSetting(_settingPath / PROJECT_SETTING_FILENAME);
    return true;
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

bool EFileSystem::IsVaildGuid(const File::Guid& guid)
{
    return _guidToPathTable.find(guid) != _guidToPathTable.end();
}

bool EFileSystem::IsValidExtension(const File::FString& ext)
{
    return (ext == "") || (_notifierTable.find(ext) != _notifierTable.end());
}

bool EFileSystem::IsSameContext(std::weak_ptr<File::Context> left, std::weak_ptr<File::Context> right)
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

const File::Path& EFileSystem::GetPathFromGuid(const File::Guid& guid)
{
    auto wpContext = GetContext(guid);
    if (false == wpContext.expired())
    {
        return wpContext.lock()->GetPath();
    }
    return NULL_PATH;
}
const File::Guid& EFileSystem::GetGuidFromPath(const File::Path& path)
{
    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        const MetaData& meta = wpContext.lock()->GetMeta();
        return meta.GetFileGuid();
    }
    return NULL_GUID;
}
std::weak_ptr<Context> EFileSystem::GetContext(const File::Guid& guid)
{
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
std::weak_ptr<Context> EFileSystem::GetContext(const File::Path& path)
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
    auto itr = _notifierTable.find(ext);
    if (itr != _notifierTable.end())
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
                TCHAR title[] = L"폴더를 선택하세요.";
                UINT  flags   = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;

                File::Path directory = _rootPath / PROJECT_SETTING_PATH;
                directory            = directory.generic_wstring();
                if (File::OpenForderBrowser(title, flags, directory, _rootPath))
                {
                    File::Path filename  = L"FileSystem.UmSetting";
                    SaveSetting(directory / filename);
                }
            }
            if (ImGui::MenuItem("Load"))
            {
                TCHAR      title[] = L"폴더를 선택하세요.";
                UINT       flags   = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
                File::Path path;
                if (File::OpenForderBrowser(title, flags, path))
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

    ImGui::SameLine();

    if (ImGui::Button("Load"))
    {
        File::Path filename  = L"fileSystem.setting";
        File::Path directory = PROJECT_SETTING_PATH;
        LoadSetting(directory / filename);
    }

    ImGui::Separator();

    ImGui::Text("FileSystem Setting");

    ImGui::Text("Debug Level: ");
    ImGui::DragInt("##DragDebuglevel", &_setting.DebugLevel, 0, 3);

    static char metaExt[128] = "";
    strcpy_s(metaExt, _setting.MetaExt.c_str());
    ImGui::Text("Medta Extension: ");
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
        _notifierTable[ext].insert(notifier);
    }
}

void EFileSystem::UnRegisterFileEventNotifier(FileEventNotifier* notifier) 
{
    const std::vector<FString> exts = notifier->GetTriggerExtensions();

     for (const auto& ext : exts)
    {
         auto itr = _notifierTable.find(ext);
        if (itr != _notifierTable.end())
        {
            auto& notifierSet = itr->second;
            notifierSet.erase(notifier);
            if (notifierSet.empty())
            {
                _notifierTable.erase(itr);
            }
        }
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
    _notifierTable.clear();
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

void EFileSystem::RegisterContext(const File::Path& srcPath) 
{
    // 파일이 없으면 return
    if (false == stdfs::exists(srcPath))
        return;

     auto find = GetContext(srcPath);

     if (false != find.expired())
     {
         std::shared_ptr<Context> context;

         auto absPath = stdfs::weakly_canonical(srcPath);

         if (true == stdfs::is_regular_file(absPath))
         {
             context = std::make_shared<FileContext>(absPath);
         }
         else if (true == stdfs::is_directory(absPath))
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
         auto& guid = meta.GetFileGuid();

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
    auto wpContext = GetContext(path);

    if (false == wpContext.expired())
    {
        auto spContext  = wpContext.lock();
        auto& meta      = spContext->GetMeta();
        auto& guid      = meta.GetFileGuid();

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
    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        auto            spContext = wpContext.lock();
        const MetaData& meta      = spContext->GetMeta();
        File::Guid      guid      = meta.GetFileGuid();

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