#include "pch.h"
#include "FileSystem.h"
#include "Extra/FileContext.h"

namespace File
{
    void FileSystem::Initialize() 
    {
        IDMapper::_ignoreExtTable.insert(MetaData::EXTENSION);

        if (false == stdfs::is_directory(_rootPath))
        {
            OutputLog(L"System RootPath Is Not Directory");
        }

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
        IDMapper::AddedFile(path);

        if (true == stdfs::is_directory(path))
        {
            for (const auto& entry : stdfs::recursive_directory_iterator(path))
            {
                // 경로를 재네릭화
                Path gPath = entry.path().generic_string();
                if (true == IDMapper::IsVaildExtension(gPath.extension()))
                {
                    ReadDiectory(gPath);
                }
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

    bool IDMapper::IsVaildGuid(const File::Guid& guid)
    {
        return _guidToPathTable.find(guid) != _guidToPathTable.end();
    }
    bool IDMapper::IsVaildExtension(const File::Path& path)
    {
        return _ignoreExtTable.find(path) == _ignoreExtTable.end();
    }
    const File::Path& IDMapper::GetPathFromGuid(const File::Guid& guid)
    {
        auto wpContext = GetContext(guid);
        if (false == wpContext.expired())
        {
            return wpContext.lock()->GetPath();
        }
        return "";
    }
    const File::Guid& IDMapper::GetGuidFromPath(const File::Path& path)
    {
        auto wpContext = GetContext(path);
        if (false == wpContext.expired())
        {
            const MetaData& meta = wpContext.lock()->GetMeta();
            return meta.GetFileGuid();
        }
        return NULL_GUID;
    }
    std::weak_ptr<Context> IDMapper::GetContext(const File::Guid& guid)
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
    std::weak_ptr<Context> IDMapper::GetContext(const File::Path& path)
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

    void IDMapper::Clear() 
    {        
        _pathToGuidTable.clear();
        _guidToPathTable.clear();
        _contextTable.clear();
    }

    void IDMapper::AddedFile(const File::Path& path) 
    {
        // 파일이 없으면 return
        if (false == stdfs::exists(path))
            return;
        // 유효한 확장자가 아니면 return
        if (false == IsVaildExtension(path.extension()))
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
            auto parentContext = IDMapper::GetContext<ForderContext>(parentPath);
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

    void IDMapper::RemovedFile(const File::Path& path) 
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
            auto wpForderContext = IDMapper::GetContext<ForderContext>(parentPath);
            if (false == wpForderContext.expired())
            {
                wpForderContext.lock()->_contextTable.erase(path.filename());
            }
        }
    }

    void IDMapper::ModifiedFile(const File::Path& path)
    {
        auto wpContext = GetContext(path);
        if (false == wpContext.expired())
        {
            wpContext.lock()->OnFileModified(path);
        }
    }

    void IDMapper::MovedFile(const File::Path& oldPath,
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
                IDMapper::GetContext<ForderContext>(oldForderPath);
            auto newForderContext =
                IDMapper::GetContext<ForderContext>(newForderPath);

            if (false == oldForderContext.expired() && false == newForderContext.expired())
            {
                oldForderContext.lock()->_contextTable.erase(oldPath.filename());
                newForderContext.lock()->_contextTable[newPath.filename()] = wpContext;
            }
        }
    }
   
} // namespace File