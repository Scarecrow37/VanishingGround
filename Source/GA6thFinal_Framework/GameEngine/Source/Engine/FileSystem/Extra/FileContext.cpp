#include "pch.h"
#include "FileContext.h"

namespace File
{
    Context::Context(const Path& path) 
        : _name(path.filename().string()), _path(path), _meta({})
    {
        if (true == LoadMeta())
        {
            if (UmFileSystem.GetDebugLevel() >= 2)
                OutputLog(L"Failed to load meta: " +
                          _meta.GetFileGuid().wstring());
        }
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
            // 차후 동작은 이벤트로 처리하므로 파일만 변경
            return File::RemoveFile(_path);
        }
        return false;
    }

    bool Context::Move(const File::Path& newPath)
    {
        if (false == _path.empty())
        {
            if (newPath != _path)
            {
                // 차후 동작은 이벤트로 처리하므로 파일만 변경
                return MoveFileExW(_path.wstring().c_str(), newPath.wstring().c_str(), MOVEFILE_REPLACE_EXISTING);
            }
        }
        return false;
    }

    FileContext::FileContext(const File::Path& path) 
        : Context(path)
    {
    }

    FileContext::~FileContext()
    {
    }

    void FileContext::OnFileAdded(const Path& path) 
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
        _name = newPath.filename().string();
        
        LoadMeta();

        //File::Path oldParentPath    = oldPath.parent_path().generic_string();
        //File::Path newParentPath    = newPath.parent_path().generic_string();
        //auto       oldParentContext = UmFileSystem.GetContext<File::FolderContext>(oldParentPath);
        //auto       newParentContext = UmFileSystem.GetContext<File::FolderContext>(newParentPath);
        //
        //if (false == oldParentContext.expired() && false == newParentContext.expired())
        //{
        //    auto spOldContext = oldParentContext.lock();
        //    auto spNewContext = newParentContext.lock();
        //    spOldContext->_contextTable.erase(oldPath.filename());
        //    spNewContext->_contextTable[newPath.filename()] = UmFileSystem.GetContext(GetPath());
        //}
    }

    void FileContext::OnFileMoved(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;
        _name = newPath.filename().string();

        LoadMeta();

        //File::Path oldParentPath    = oldPath.parent_path().generic_string();
        //File::Path newParentPath    = newPath.parent_path().generic_string();
        //auto       oldParentContext = UmFileSystem.GetContext<File::FolderContext>(oldParentPath);
        //auto       newParentContext = UmFileSystem.GetContext<File::FolderContext>(newParentPath);
        //
        //if (false == oldParentContext.expired() && false == newParentContext.expired())
        //{
        //    auto spOldContext = oldParentContext.lock();
        //    auto spNewContext = newParentContext.lock();
        //    spOldContext->_contextTable.erase(oldPath.filename());
        //    spNewContext->_contextTable[newPath.filename()] = UmFileSystem.GetContext(GetPath());
        //}
    }

    FolderContext::FolderContext(const File::Path& path) 
    : Context(path)
    {

    }
    
    FolderContext::~FolderContext() 
    {
    
    }

    void FolderContext::OnFileAdded(const Path& path) 
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
                UmFileSystem.RemovedFile(wpContext.lock()->GetPath());
            }
        }
    }

    void FolderContext::OnFileRenamed(const Path& oldPath, const Path& newPath)
    {
        _path = newPath;
        _name = newPath.filename().string();

        LoadMeta();

        for (auto& [name, wpContext] : _contextTable)
        {
            if (false == wpContext.expired())
            {
                auto        spContext      = wpContext.lock();
                const Path& oldContextPath = spContext->GetPath();
                const Path& newContextPath = newPath / spContext->GetPath().filename();
                UmFileSystem.MovedFile(oldContextPath.generic_string(), newContextPath.generic_string());
            }
        }
    }

    void FolderContext::OnFileMoved(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;
        _name = newPath.filename().string();

        LoadMeta();

        for (auto& [name, wpContext] : _contextTable)
        {
            if (false == wpContext.expired())
            {
                auto        spContext      = wpContext.lock();
                const Path& oldContextPath = spContext->GetPath();
                const Path& newContextPath = newPath / spContext->GetPath().filename();
                UmFileSystem.MovedFile(oldContextPath, newContextPath);
            }
        }
    }

} // namespace File