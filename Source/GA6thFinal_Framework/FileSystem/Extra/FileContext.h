#pragma once

namespace File
{
    class FileContext;
    class ForderContext;

    class Context : public IFileEventProcesser
    {
    public:
        Context(const Path& path);
        virtual ~Context();

    public:
    virtual bool IsDirectory() = 0;
    virtual bool IsRegularFile() = 0;
    public:
        bool LoadMeta();
        template <typename T>
        T* SafeCast()
        {
            if constexpr (std::is_same_v<T, ForderContext>)
            {
                if (true == IsDirectory())
                    return static_cast<ForderContext*>(this);
            }
            else if constexpr (std::is_same_v<T, FileContext>)
            {
                if (true == IsRegularFile())
                    return static_cast<FileContext*>(this);
            }
            return nullptr;
        }
    public:
        inline const Path&     GetPath() const { return _path; }
        inline const MetaData& GetMeta() const { return _meta; }
    protected:
        Path _path;
        MetaData _meta;
    };

    class FileContext 
        : public Context
    {
    public:
        FileContext(const File::Path& path);
        virtual ~FileContext();
        FileContext(const FileContext&) = delete;
        FileContext& operator=(const FileContext&) = delete;
        FileContext(FileContext&&) = delete;
        FileContext& operator=(FileContext&&) = delete;

    public:
        inline virtual bool IsDirectory() override { return false; }
        inline virtual bool IsRegularFile() override { return true; }
    public:

        void SetEventProcessor(IFileEventProcesser* eventProcessor)
        {
            _eventProcessor = eventProcessor;
        }

    public:
        virtual void OnFileAdded(const Path& path) override;
        virtual void OnFileModified(const Path& path) override;
        virtual void OnFileRemoved(const Path& path) override;
        virtual void OnFileRenamed(const Path& oldPath, const Path& newPath) override;
        virtual void OnFileMoved(const Path& oldPath, const Path& newPath) override;
    private:
        IFileEventProcesser*    _eventProcessor;  
    };

    class ForderContext
        : public Context
    {
        friend class Context; 
        friend class FileContext; 
    public:
        ForderContext(const File::Path& path);
        virtual ~ForderContext();
        ForderContext(const ForderContext&)            = delete;
        ForderContext& operator=(const ForderContext&) = delete;
        ForderContext(ForderContext&&)                 = delete;
        ForderContext& operator=(ForderContext&&)      = delete;

    public:
        inline virtual bool IsDirectory() override { return true; }
        inline virtual bool IsRegularFile() override { return false; }
    public:
        virtual void OnFileAdded(const Path& path) override;
        virtual void OnFileModified(const Path& path) override;
        virtual void OnFileRemoved(const Path& path) override;
        virtual void OnFileRenamed(const Path& oldPath, const Path& newPath) override;
        virtual void OnFileMoved(const Path& oldPath, const Path& newPath) override;

    public:

    public:
        /* 이름에 맞는 폴더를 생성합니다. */
        bool CreateFolder(const Path& name);

        /* 폴더, 파일을 삭제합니다. */
        bool RemoveContext(const Path& name);
    public:
        std::unordered_map<FString, std::weak_ptr<Context>> _contextTable;
        auto begin() { return _contextTable.begin(); }
        auto end() { return _contextTable.end(); }
    };
}