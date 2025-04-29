#include "pch.h"
#include "FileContext.h"

namespace File
{
    Context::Context(const Path& path) 
        : _name(path.filename().string()), _path(path), _meta({})
    {
        if (UmFileSystem.GetDebugLevel() >= 3)
            OutputLog(L"Create Context: " + _path.wstring());
    }

    Context::~Context() 
    {
#ifdef _DEBUG
        if (UmFileSystem.GetDebugLevel() >= 3)
            OutputLog(L"Destroy Context: " + _path.wstring());
#endif // DEBUG

    }

    bool Context::LoadMeta()
    {
        // 파일과 메타의 경로(확장자)를 헷갈리지 말자
        File::Path metaPath = _path;
        metaPath.replace_extension(UmFileSystem.GetMetaExt());

        // 처음엔 어차피 Null이므로 false가 반환된다.
        if (false == _meta.Move(metaPath))
        {
            // 로드가 성공하지 못하면 Create
            if (false == _meta.Load(metaPath))
            {
                _meta.Create(metaPath);
            }
        }

        // 마지막으로 Null여부를 반환
        return (false == _meta.IsNull());
    }

    bool Context::Open() 
    {
        if (false == _path.empty())
        {
            return File::OpenFile(_path);
        }
        return false;
    }

    bool Context::Remove() 
    {
        if (false == _path.empty())
        {
            if (true == fs::exists(_path))
            {
                // 차후 동작은 이벤트로 처리하므로 파일만 변경
                return File::RemoveFile(_path);
            }
        }
        return false;
    }

    bool Context::Move(const File::Path& newPath)
    {
        if (false == _path.empty())
        {
            if (newPath != _path)
            {
                File::Path from = _path;
                File::Path to = GenerateUniquePath(newPath);
                fs::rename(from, to);
                return true;
            }
        }
        return false;
    }

    std::weak_ptr<FolderContext> Context::GetParentContext()
    {
        File::Path parentPath = _path.parent_path();
        auto       context    = UmFileSystem.GetContext<FolderContext>(parentPath);
        return context;
    }

    FileContext::FileContext(const File::Path& path) 
        : Context(path)
    {
        if (true == IsRegularFile())
        {
            if (true == LoadMeta())
            {
                if (UmFileSystem.GetDebugLevel() >= 2)
                    OutputLog(L"Failed to load meta: " + _meta.GetFileGuid().wstring());
            }
        }
    }

    FileContext::~FileContext()
    {
    }

    void FileContext::OnFileRegistered(const File::Path& path) 
    {
    }

    void FileContext::OnFileUnregistered(const File::Path& path) 
    {
    }

    void FileContext::OnFileModified(const Path& path) 
    {
    }

    void FileContext::OnFileRemoved(const Path& path) 
    {
        _meta.Remove();
    }

    void FileContext::OnFileRenamed(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;
        _name = _path.filename().string();
        
        LoadMeta();
    }

    void FileContext::OnFileMoved(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;
        _name = _path.filename().string();

        LoadMeta();
    }

    FolderContext::FolderContext(const File::Path& path) 
    : Context(path)
    {

    }
    
    FolderContext::~FolderContext() 
    {
    
    }

    void FolderContext::MoveContext(std::weak_ptr<Context> context) 
    {
        //if (false == context.expired())
        //{
        //    auto spContext = context.lock();
        //    auto parentContext = spContext->GetParentContext();
        //    if (false == parentContext.expired())
        //    {
        //        auto spParentContext = parentContext.lock();
        //        
        //        spParentContext->_contextTable.find
        //        File::Path name = spContext->GetPath().filename();
        //        _contextTable[name] = context;
        //    }
        //
        //    const Path& oldPath                 = spContext->GetPath();
        //    const Path  newPath                 = _path / spContext->GetPath().filename();
        //    _contextTable[spContext->GetName()] = context;
        //    spContext->Move(newPath);
        //}
    }

    void FolderContext::OnFileRegistered(const File::Path& path)
    {
    }

    void FolderContext::OnFileUnregistered(const File::Path& path) 
    {
    }

    void FolderContext::OnFileModified(const Path& path) 
    {
    }

    void FolderContext::OnFileRemoved(const Path& path) 
    {
        _meta.Remove();

        for (auto& [name, wpContext] : _contextTable)
        {
            if (false == wpContext.expired())
            {
                UmFileSystem.ProcessRemovedFile(wpContext.lock()->GetPath());
            }
        }
    }

    void FolderContext::OnFileRenamed(const Path& oldPath, const Path& newPath)
    {
        _path = newPath;
        _name = _path.filename().string();

        LoadMeta();

        std::vector<std::pair<FString, FString>>  table;

        for (auto& [name, wpContext] : _contextTable)
        {
            if (false == wpContext.expired())
            {
                auto        spContext      = wpContext.lock();
                const Path& oldContextPath = spContext->GetPath();
                const Path newContextPath = _path / spContext->GetPath().filename();
                table.push_back(std::make_pair(oldContextPath.generic_string(), newContextPath.generic_string()));
            }
        }
        for (auto& [oldPath, newPath] : table)
        {
            UmFileSystem.ProcessMovedFile(oldPath, newPath);
        }
    }

    void FolderContext::OnFileMoved(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;
        _name = _path.filename().string();

        LoadMeta();

        std::vector<std::pair<FString, FString>> table;

        for (auto& [name, wpContext] : _contextTable)
        {
            if (false == wpContext.expired())
            {
                auto        spContext      = wpContext.lock();
                const Path& oldContextPath = spContext->GetPath();
                const Path newContextPath = _path / spContext->GetPath().filename();
                table.push_back(std::make_pair(oldContextPath.generic_string(), newContextPath.generic_string()));
            }
        }
        for (auto& [oldPath, newPath] : table)
        {
            UmFileSystem.ProcessMovedFile(oldPath, newPath);
        }
    }

} // namespace File