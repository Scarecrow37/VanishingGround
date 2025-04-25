#pragma once

namespace File
{
    class FileContext;
    class FolderContext;

    class Context : public Interface::IFileEventProcesser
    {
    public:
        Context(const Path& path);
        virtual ~Context();

    public:
    virtual bool IsDirectory() = 0;
    virtual bool IsRegularFile() = 0;

    public:
        bool LoadMeta();

        bool Open();

        bool Remove();

        /* 매개 변수: 폴더 경로X, 최종 경로O */
        bool Move(const File::Path& newPath);

        std::weak_ptr<FolderContext> GetParentContext();

        template <typename T>
        T* SafeCast()
        {
            if constexpr (std::is_same_v<T, FolderContext>)
            {
                if (true == IsDirectory())
                    return static_cast<FolderContext*>(this);
            }
            else if constexpr (std::is_same_v<T, FileContext>)
            {
                if (true == IsRegularFile())
                    return static_cast<FileContext*>(this);
            }
            return nullptr;
        }

    public:
        inline const std::string&   GetName() const { return _name; }
        inline const Path&          GetPath() const { return _path; }
        inline const MetaData&      GetMeta() const { return _meta; }

    protected:
        std::string _name;
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
        virtual void OnFileRegistered(const File::Path& path) override;
        virtual void OnFileUnregistered(const File::Path& path) override;
        virtual void OnFileModified(const Path& path) override;
        virtual void OnFileRemoved(const Path& path) override;
        virtual void OnFileRenamed(const Path& oldPath, const Path& newPath) override;
        virtual void OnFileMoved(const Path& oldPath, const Path& newPath) override;
    };

    class FolderContext
        : public Context
    {
        friend class Context; 
        friend class FileContext; 
    public:
        FolderContext(const File::Path& path);
        virtual ~FolderContext();
        FolderContext(const FolderContext&)            = delete;
        FolderContext& operator=(const FolderContext&) = delete;
        FolderContext(FolderContext&&)                 = delete;
        FolderContext& operator=(FolderContext&&)      = delete;

    public:
        auto begin() { return _contextTable.begin(); }
        auto end() { return _contextTable.end(); }

    public:
        inline virtual bool IsDirectory() override { return true; }
        inline virtual bool IsRegularFile() override { return false; }

    public:
        void MoveContext(std::weak_ptr<Context> context);

    public:
        virtual void OnFileRegistered(const File::Path& path) override;
        virtual void OnFileUnregistered(const File::Path& path) override;
        virtual void OnFileModified(const Path& path) override;
        virtual void OnFileRemoved(const Path& path) override;
        virtual void OnFileRenamed(const Path& oldPath, const Path& newPath) override;
        virtual void OnFileMoved(const Path& oldPath, const Path& newPath) override;

    public:
        std::unordered_map<FString, std::weak_ptr<Context>> _contextTable;
        
    };
}