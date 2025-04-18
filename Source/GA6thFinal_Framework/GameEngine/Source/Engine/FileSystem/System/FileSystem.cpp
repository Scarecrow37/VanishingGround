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
        Reload(); 
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

void EFileSystem::Reload() 
{
    _pathToGuidTable.clear();
    _guidToPathTable.clear();
    _contextTable.clear();

    ReadDirectory(_setting.RootPath);
}

void EFileSystem::Clear()
{        
    _pathToGuidTable.clear();
    _guidToPathTable.clear();
    _contextTable.clear();
    _notifierTable.clear();
}

void EFileSystem::ReadDirectory(const File::Path& path) 
{
    AddedFile(path);

    if (true == std::filesystem::is_directory(path))
    {
        for (const auto& entry :
             std::filesystem::recursive_directory_iterator(path))
        {
            // 경로를 재네릭화
            File::Path genericPath = entry.path().generic_string();
            if (true == IsValidExtension(genericPath.extension()))
            {
                ReadDirectory(genericPath);
            }
        }
    }
}

void EFileSystem::AddedFile(const File::Path& path) 
{
    // 파일이 없으면 return
    if (false == stdfs::exists(path))
        return;

    auto find = GetContext(path);

    if (false != GetContext(path).expired())
    {
        std::shared_ptr<Context> context;

        if (true == stdfs::is_regular_file(path))
        {
            context = std::make_shared<FileContext>(path);
        }
        else if (true == stdfs::is_directory(path))
        {
            context = std::make_shared<ForderContext>(path);
        }
        else
        {
            return;
        }

        // 부모 폴더에서 자신을 추가한다.
        File::Path parentPath = path.parent_path().generic_string();
        auto parentContext = UmFileSystem.GetContext<ForderContext>(parentPath);
        if (false == parentContext.expired())
        {
            parentContext.lock()->_contextTable[path.filename()] = context;
        }

        const MetaData& meta = context->GetMeta();
        File::Guid      guid = meta.GetFileGuid();

        _pathToGuidTable[path] = context;
        _guidToPathTable[guid] = context;
        _contextTable.insert(context);

        context->OnFileAdded(path);      

        auto notifierSet = GetNotifiers(path.extension());
        for (auto& notifier : notifierSet)
        {
            notifier->OnFileAdded(path);
        } 
    }
}

void EFileSystem::RemovedFile(const File::Path& path) 
{
    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        auto            spContext = wpContext.lock();
        const MetaData& meta      = spContext->GetMeta();
        File::Guid      guid = meta.GetFileGuid();

        spContext->OnFileRemoved(path);

        _pathToGuidTable.erase(path);
        _guidToPathTable.erase(guid);
        _contextTable.erase(spContext);

        // 부모 폴더에서 자신을 제거한다.
        File::Path parentPath = path.parent_path().generic_string();
        auto wpForderContext = EFileSystem::GetContext<ForderContext>(parentPath);
        if (false == wpForderContext.expired())
        {
            wpForderContext.lock()->_contextTable.erase(path.filename());
        }

        auto notifierSet = GetNotifiers(path.extension());
        for (auto& notifier : notifierSet)
        {
            notifier->OnFileRemoved(path);
        } 
    }
}

void EFileSystem::ModifiedFile(const File::Path& path)
{
    auto wpContext = GetContext(path);
    if (false == wpContext.expired())
    {
        wpContext.lock()->OnFileModified(path);

        auto notifierSet = GetNotifiers(path.extension());
        for (auto& notifier : notifierSet)
        {
            notifier->OnFileModified(path);
        } 
    }
}

void EFileSystem::MovedFile(const File::Path& oldPath,
                            const File::Path& newPath) 
{
    // 이전 경로에서 컨텍스트를 찾아 새 경로로 옮기기
    auto wpContext = GetContext(oldPath);
    if (false == wpContext.expired())
    {
        _pathToGuidTable.erase(oldPath);
        _pathToGuidTable[newPath] = wpContext;
        // Guid는 동일하므로 안지워도 된다.

        if (oldPath.parent_path() == newPath.parent_path())
        {
            // 같은 폴더 내에서 이름만 변경
            wpContext.lock()->OnFileRenamed(oldPath, newPath);
        }
        else
        {
            // 다른 폴더로 이동
            wpContext.lock()->OnFileMoved(oldPath, newPath);
        }

        // 폴더 컨텍스트에게도 알려준다.
        File::Path oldForderPath = oldPath.parent_path().generic_string();
        File::Path newForderPath = newPath.parent_path().generic_string();

        auto oldForderContext =
            UmFileSystem.GetContext<ForderContext>(oldForderPath);
        auto newForderContext =
            UmFileSystem.GetContext<ForderContext>(newForderPath);

        if (false == oldForderContext.expired() && false == newForderContext.expired())
        {
            oldForderContext.lock()->_contextTable.erase(oldPath.filename());
            newForderContext.lock()->_contextTable[newPath.filename()] = wpContext;
        }

        if (oldPath.parent_path() == newPath.parent_path())
        {
            {
                auto notifierSet = GetNotifiers(oldPath.extension());
                for (auto& notifier : notifierSet)
                {
                    notifier->OnFileRenamed(oldPath, newPath);
                } 
            }
            {
                auto notifierSet = GetNotifiers(newPath.extension());
                for (auto& notifier : notifierSet)
                {
                    notifier->OnFileMoved(oldPath, newPath);
                } 
            }
        }
        else
        {
            auto notifierSet = GetNotifiers(newPath.extension());
            for (auto& notifier : notifierSet)
            {
                notifier->OnFileRenamed(oldPath, newPath);
            } 
        }
    }
}