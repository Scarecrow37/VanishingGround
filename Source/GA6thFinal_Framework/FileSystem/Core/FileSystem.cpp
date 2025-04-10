#include "pch.h"
#include "FileSystem.h"
#include "Extra/FileContext.h"

namespace File
{
    void FileSystem::Initialize() 
    {
        IDMapper::_ignoreExtTable.insert(MetaData::EXTANSTION);

        ReadDiectory(_rootPath);

        _observer = new FileObserver();
        _observer->Start(_rootPath, [this](const FileEventData& event) {
            RecieveFileEvents(event);
        });
    }
    void FileSystem::DeInitialize() 
    {
        if (nullptr != _observer)
        {
            _observer->Stop();
            delete _observer;
            _observer = nullptr;
        }
    }
    void FileSystem::Update()
    {
        ProcessEventQueue();
    }
    void FileSystem::ReadDiectory(const Path& path)
    {
        if (false == stdfs::is_directory(path))
        {
            return;
        }

        for (const auto& entry : stdfs::recursive_directory_iterator(path))
        {
            Path gPath = entry.path().generic_string();
            if (true == stdfs::is_regular_file(gPath) &&
                true == IDMapper::IsVaildExtension(gPath.extension()))
            {
                IDMapper::AddedFile(gPath);
            }
            if (true == stdfs::is_directory(entry))
            {
                ReadDiectory(gPath);
            }
        }
    }

    void FileSystem::RecieveFileEvents(const FileEventData& data) 
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _eventQueue.push_back(data);
    }
    void FileSystem::ProcessEventQueue() 
    {
        std::lock_guard<std::mutex> lock(_mutex);

        for (const FileEventData& event : _eventQueue)
        {
            const auto& [lParam, rParam, eventType] = event;

            Path lp = (_rootPath / lParam).generic_string();
            Path rp = (_rootPath / rParam).generic_string();

            switch (eventType)
            {
            case EventType::ADDED:
                IDMapper::AddedFile(lp);
                break;
            case EventType::REMOVED:
                IDMapper::RemovedFile(lp);
                break;
            case EventType::MODIFIED:
                IDMapper::ModifiedFile(lp);
                break;
            case EventType::RENAMED:
                IDMapper::MovedFile(lp, rp);
                break;
            case EventType::MOVED:
                IDMapper::MovedFile(lp, rp);
                break;
            default:
                break;
            }
        }

        _eventQueue.clear();
    }

    const Path& FileSystem::GetRootPath() const
    {
        return _rootPath;
    }


    const File::Path& IDMapper::GetPathFromGuid(const File::Guid& guid)
    {
        FileContext* context = GetFileContext(guid);
        if (nullptr != context)
        {
            return context->GetPath();
        }
        return "";
    }
    const File::Guid& IDMapper::GetGuidFromPath(const File::Path& path)
    {
        FileContext* context = GetFileContext(path);
        if (nullptr != context)
        {
            const MetaData& meta = context->GetMeta();
            return meta.GetFileGuid();
        }
        return NULL_GUID;
    }
    FileContext* IDMapper::GetFileContext(const File::Guid& guid)
    {
        auto itr = _guidToPathTable.find(guid);
        if (itr != _guidToPathTable.end())
        {
            return itr->second;
        }
        return nullptr;
    }
    FileContext* IDMapper::GetFileContext(const File::Path& path)
    {
        auto itr = _pathToGuidTable.find(path);
        if (itr != _pathToGuidTable.end())
        {
            return itr->second;
        }
        return nullptr;
    }

    bool IDMapper::IsVaildGuid(const File::Guid& guid)
    {
        return _guidToPathTable.find(guid) != _guidToPathTable.end();
    }

    bool IDMapper::IsVaildExtension(const File::Path& path)
    {
        return _ignoreExtTable.find(path) == _ignoreExtTable.end();
    }

    void IDMapper::Clear() 
    {
        for (auto& context : _pathToGuidTable)
        {
            delete context.second;
        }
        _pathToGuidTable.clear();
        _guidToPathTable.clear();
    }

    void IDMapper::AddedFile(const File::Path& path) 
    {
        // 파일이 없으면 return;
        if (false == stdfs::exists(path))
            return;
        if (false == IsVaildExtension(path.extension()))
            return;

        FileContext* context = GetFileContext(path);
        if (nullptr == context)
        {
            context = new FileContext(path);

            const MetaData& meta    = context->GetMeta();
            File::Guid      guid    = meta.GetFileGuid();

            IDMapper::_pathToGuidTable[path] = context;
            IDMapper::_guidToPathTable[guid] = context;

            context->OnFileAdded(path);
        }
    }

    void IDMapper::RemovedFile(const File::Path& path) 
    {
        FileContext* context = GetFileContext(path);
        if (nullptr != context)
        {
             File::Guid guid = context->GetMeta().GetFileGuid();

             _pathToGuidTable.erase(path);
             _guidToPathTable.erase(guid);

             context->OnFileRemoved(path);

             delete context;
        }
    }

    void IDMapper::ModifiedFile(const File::Path& path)
    {
        FileContext* context = GetFileContext(path);
        if (nullptr != context)
        {
            context->OnFileModified(path);
        }
    }

    void IDMapper::MovedFile(const File::Path& oldPath, const File::Path& newPath) 
    {
        // 이전 경로에서 컨텍스트를 찾아 새 경로로 옮기기
        FileContext* context = GetFileContext(oldPath);
        if (nullptr != context)
        {
            _pathToGuidTable.erase(oldPath);
            _pathToGuidTable[newPath] = context;
            // Guid는 동일하므로 안지워도 된다.

            if (oldPath.parent_path() == newPath.parent_path())
            {
                // 같은 폴더 내에서 이름만 변경
                context->OnFileRenamed(oldPath, newPath);
            }
            else
            {
                // 다른 폴더로 이동
                context->OnFileMoved(oldPath, newPath);
            }
        }
    }
   
} // namespace File