#include "pch.h"
#include "FileSystem.h"

using namespace File;

bool EFileSystem::SaveSetting(const File::Path& path)
{
    auto setting = rfl::yaml::save(path.string(), _setting);
    if (false == setting)
    {
        return false;
    }
    else
    {
        return true;
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
        ReadDirectory(); 
        return true;
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
    if (ImGui::Button("Save"))
    {
        File::Path filename  = L"fileSystem.setting";
        File::Path directory = PROJECT_SETTING_PATH;
        SaveSetting(directory / filename);
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

    static char path[128] = "";
    strcpy_s(path, _setting.RootPath.c_str());
    ImGui::Text("Save Path: ");
    if (ImGui::InputText("##InputRootPath", path, IM_ARRAYSIZE(path)))
    {
        _setting.RootPath = path;
    }

    static char metaExt[128] = "";
    strcpy_s(metaExt, _setting.MetaExt.c_str());
    ImGui::Text("Medta Extension: ");
    if (ImGui::InputText("##InputMetaExt", metaExt, IM_ARRAYSIZE(metaExt)))
    {
        _setting.MetaExt = metaExt;
    }
}

void EFileSystem::RegisterFileEventNotifier(
    FileEventNotifier*                        notifier,
    const std::initializer_list<std::string>& exts)
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
    ReadDirectory(_setting.RootPath);
}

void EFileSystem::ReadDirectory(const File::Path& path) 
{
    RegisterContext(path);

    if (true == fs::is_directory(path))
    {
        for (const auto& entry : fs::recursive_directory_iterator(path))
        {
            // 경로를 재네릭화
            File::Path genericPath = entry.path().generic_string();
            File::Path extesion    = genericPath.extension();
            if (true == IsValidExtension(extesion))
            {
                ReadDirectory(genericPath);
            }
        }
    }
}

void EFileSystem::RegisterContext(const File::Path& path) 
{
    // 파일이 없으면 return
    if (false == stdfs::exists(path))
        return;

     auto find = GetContext(path);

     if (false != find.expired())
     {
         std::shared_ptr<Context> context;

         if (true == stdfs::is_regular_file(path))
         {
             context = std::make_shared<FileContext>(path);
         }
         else if (true == stdfs::is_directory(path))
         {
             context = std::make_shared<FolderContext>(path);
         }
         else
         {
             return;
         }

         auto& meta = context->GetMeta();
         auto& guid = meta.GetFileGuid();

         _pathToGuidTable[path] = context;
         _guidToPathTable[guid] = context;
         _contextTable.insert(context);

         // 부모 폴더에서 자신을 추가한다.
         File::Path parentPath    = path.parent_path().generic_string();
         auto       parentContext = UmFileSystem.GetContext<FolderContext>(parentPath);
         if (false == parentContext.expired())
         {
             File::Path filename = path.filename();
             parentContext.lock()->_contextTable[filename] = context;
         }

         context->OnFileRegistered(path);

         File::Path extension    = path.extension();
         NotifierSet notifierSet = GetNotifiers(extension);
         for (auto& notifier : notifierSet)
         {
             notifier->OnFileRegistered(path);
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