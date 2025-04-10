#pragma once

namespace File
{
    class FileContext 
        : public IFileEventProcesser
    {
    public:
        FileContext(const File::Path& path);
        ~FileContext();
        FileContext(const FileContext&) = delete;
        FileContext& operator=(const FileContext&) = delete;
        FileContext(FileContext&&) = delete;
        FileContext& operator=(FileContext&&) = delete;
    public:
        bool LoadMeta();

        void SetEventProcessor(IFileEventProcesser* eventProcessor)
        {
            _eventProcessor = eventProcessor;
        }

    public:
        virtual void OnFileAdded(const Path& path) override;
        virtual void OnFileModified(const Path& path) override;
        virtual void OnFileRemoved(const Path& path) override;
        virtual void OnFileRenamed(const Path& newPath, const Path& oldPath) override;
        virtual void OnFileMoved(const Path& newPath, const Path& oldPath) override;
    public:
        inline const Path&      GetPath() const { return _path; }
        inline const MetaData&  GetMeta() const { return _meta; }
    private:
        Path                    _path;
        MetaData                _meta;
        IFileEventProcesser*    _eventProcessor;  
    };
}