#pragma once

namespace File
{
    class FileEventNotifier 
        : public IFileEventProcesser
    {
    public:
        FileEventNotifier(const std::initializer_list<std::string>& exts);
        virtual ~FileEventNotifier();

    public:
        void OnFileAdded(const Path& path) override = 0;
        void OnFileModified(const Path& path) override = 0;
        void OnFileRemoved(const Path& path) override  = 0;
        void OnFileRenamed(const Path& oldPath, const Path& newPath) override = 0;
        void OnFileMoved(const Path& oldPath, const Path& newPath) override = 0;
    public:
        bool IsTriggerExtension(const FString& ext);
        std::vector<FString> GetTriggerExtensions() const;
    private:
        std::unordered_set<FString> _triggerExtTable;
    };
}

