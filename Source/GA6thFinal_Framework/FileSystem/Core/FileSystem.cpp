#include "pch.h"
#include "FileSystem.h"
#include "Extra/FileContext.h"

namespace File
{
    void FileSystem::SetDebugLevel(int level) 
    {
        _debugLevel = level;
    }

    int FileSystem::GetDebugLevel()
    {
        return _debugLevel;
    }

    bool FileSystem::IsVaildGuid(const File::Guid& guid)
    {
        return _guidToPathTable.find(guid) != _guidToPathTable.end();
    }

    bool FileSystem::IsValidExtension(const File::FString& ext)
    {
        return (ext == "") || (_notifierTable.find(ext) != _notifierTable.end());
    }

    const File::Path& FileSystem::GetPathFromGuid(const File::Guid& guid)
    {
        auto wpContext = GetContext(guid);
        if (false == wpContext.expired())
        {
            return wpContext.lock()->GetPath();
        }
        return "";
    }
    const File::Guid& FileSystem::GetGuidFromPath(const File::Path& path)
    {
        auto wpContext = GetContext(path);
        if (false == wpContext.expired())
        {
            const MetaData& meta = wpContext.lock()->GetMeta();
            return meta.GetFileGuid();
        }
        return NULL_GUID;
    }
    std::weak_ptr<Context> FileSystem::GetContext(const File::Guid& guid)
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
    std::weak_ptr<Context> FileSystem::GetContext(const File::Path& path)
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

    std::unordered_set<File::FileEventNotifier*> FileSystem::GetNotifiers(
        const File::FString& ext)
    {
        auto itr = _notifierTable.find(ext);
        if (itr != _notifierTable.end())
        {
            return itr->second;
        }
        return std::unordered_set<File::FileEventNotifier*>();
    }

    void FileSystem::RegisterFileEventNotifier(FileEventNotifier* notifier) 
    {
        const std::vector<FString> exts = notifier->GetTriggerExtensions();
        for (const auto& ext : exts)
        {
            _notifierTable[ext].insert(notifier);
        }
    }

    void FileSystem::UnRegisterFileEventNotifier(FileEventNotifier* notifier) 
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

    void FileSystem::Clear() 
    {        
        _pathToGuidTable.clear();
        _guidToPathTable.clear();
        _contextTable.clear();
        _notifierTable.clear();
    }

    void FileSystem::ReadDirectory(const File::Path& path) 
    {
        File::FileSystem::AddedFile(path);

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

    void FileSystem::AddedFile(const File::Path& path) 
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
            auto parentContext = FileSystem::GetContext<ForderContext>(parentPath);
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
        }
    }

    void FileSystem::RemovedFile(const File::Path& path) 
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
            auto wpForderContext = FileSystem::GetContext<ForderContext>(parentPath);
            if (false == wpForderContext.expired())
            {
                wpForderContext.lock()->_contextTable.erase(path.filename());
            }
        }
    }

    void FileSystem::ModifiedFile(const File::Path& path)
    {
        auto wpContext = GetContext(path);
        if (false == wpContext.expired())
        {
            wpContext.lock()->OnFileModified(path);
        }
    }

    void FileSystem::MovedFile(const File::Path& oldPath,
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
                FileSystem::GetContext<ForderContext>(oldForderPath);
            auto newForderContext =
                FileSystem::GetContext<ForderContext>(newForderPath);

            if (false == oldForderContext.expired() && false == newForderContext.expired())
            {
                oldForderContext.lock()->_contextTable.erase(oldPath.filename());
                newForderContext.lock()->_contextTable[newPath.filename()] = wpContext;
            }
        }
    }
   
} // namespace File