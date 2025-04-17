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
        
        LoadMeta();
    }

    void FileContext::OnFileMoved(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;

        LoadMeta();
    }

    ForderContext::ForderContext(const File::Path& path) 
    : Context(path)
    {

    }
    
    ForderContext::~ForderContext() 
    {
    
    }

    void ForderContext::OnFileAdded(const Path& path) 
    {
    }

    void ForderContext::OnFileModified(const Path& path) 
    {
    }

    void ForderContext::OnFileRemoved(const Path& path) 
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

    void ForderContext::OnFileRenamed(const Path& oldPath, const Path& newPath)
    {
        _path = newPath;

        for (auto& [name, wpContext] : _contextTable)
        {
            if (false == wpContext.expired())
            {
                auto        spContext      = wpContext.lock();
                const Path& oldContextPath = spContext->GetPath();
                const Path& newContextPath = _path / spContext->GetPath().filename();
                UmFileSystem.MovedFile(oldContextPath, newContextPath);
            }
        }
    }

    void ForderContext::OnFileMoved(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;

        for (auto& [name, wpContext] : _contextTable)
        {
            if (false == wpContext.expired())
            {
                auto        spContext      = wpContext.lock();
                const Path& oldContextPath = spContext->GetPath();
                const Path& newContextPath = _path / spContext->GetPath().filename();
                UmFileSystem.MovedFile(oldContextPath, newContextPath);
            }
        }
    }

} // namespace File