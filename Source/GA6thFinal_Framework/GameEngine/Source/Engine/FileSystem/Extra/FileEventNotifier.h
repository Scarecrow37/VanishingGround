#pragma once

namespace File
{
    /*
    해당 클래스를 상속받은 후, 콜백 이벤트 함수를 구현하여 사용한다.
    FileSystem에게 Notifier를 등록하면, 해당 이벤트가 발생할 때마다
    해당 이벤트 함수를 호출한다.
    */
    class FileEventNotifier 
        : public IFileEventProcesser
    {
        friend class EFileSystem;
    public:
        FileEventNotifier();
        virtual ~FileEventNotifier();

    public:
        /* 콜백 파일 이벤트. */
        virtual void OnFileAdded(const File::Path& path) override                     = 0;
        virtual void OnFileModified(const File::Path& path) override                  = 0;
        virtual void OnFileRemoved(const File::Path& path) override                    = 0;
        virtual void OnFileRenamed(const File::Path& oldPath, const Path& newPath) override = 0;
        virtual void OnFileMoved(const File::Path& oldPath, const Path& newPath) override   = 0;

    public:
        bool IsTriggerExtension(const File::FString& ext);
        std::vector<FString> GetTriggerExtensions() const;

    private:
        std::unordered_set<FString> _triggerExtTable;
    };
}

