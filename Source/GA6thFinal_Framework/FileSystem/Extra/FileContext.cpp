#include "pch.h"
#include "FileContext.h"

namespace File
{
    Context::Context(const Path& path) 
        : _path(path), _meta({})
    {
        if (true == LoadMeta())
        {
        }
    }

    Context::~Context() 
    {
    }

    bool Context::LoadMeta()
    {
        // 파일과 메타의 경로(확장자)를 헷갈리지 말자
        File::Path metaPath = _path;
        metaPath.replace_extension(MetaData::EXTENSION);

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
        : Context(path), _eventProcessor(nullptr)
    {
    }

    FileContext::~FileContext()
    {
        if (nullptr != _eventProcessor)
        {
            delete _eventProcessor;
            _eventProcessor = nullptr;
        }
    }

    void FileContext::OnFileAdded(const Path& path) 
    {
        if (nullptr != _eventProcessor)
        {
            _eventProcessor->OnFileAdded(path);
        }
    }

    void FileContext::OnFileModified(const Path& path) 
    {
        if (nullptr != _eventProcessor)
        {
            _eventProcessor->OnFileModified(path);
        }
    }

    void FileContext::OnFileRemoved(const Path& path) 
    {
        _meta.Remove();

        if (nullptr != _eventProcessor)
        {
            _eventProcessor->OnFileRemoved(path);
        }
    }

    void FileContext::OnFileRenamed(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;
        
        LoadMeta();

        if (nullptr != _eventProcessor)
        {
            _eventProcessor->OnFileRenamed(oldPath, newPath);
        }
    }

    void FileContext::OnFileMoved(const Path& oldPath, const Path& newPath) 
    {
        _path = newPath;

        LoadMeta();

        if (nullptr != _eventProcessor)
        {
            _eventProcessor->OnFileMoved(oldPath, newPath);
        }
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
                FileSystem::RemovedFile(wpContext.lock()->GetPath());
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
                FileSystem::MovedFile(oldContextPath, newContextPath);
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
                FileSystem::MovedFile(oldContextPath, newContextPath);
            }
        }
    }

} // namespace File