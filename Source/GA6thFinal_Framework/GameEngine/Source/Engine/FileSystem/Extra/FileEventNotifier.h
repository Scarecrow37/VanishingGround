#pragma once

namespace File
{
    /*
    해당 클래스를 상속받은 후, 콜백 이벤트 함수를 구현하여 사용한다.
    FileSystem에게 Notifier를 등록하면, 해당 관련 이벤트가 발생할 때마다
    해당 이벤트 함수를 호출한다.
    */
    class FileEventNotifier 
        : public Interface::IFileEventProcesser
    {
        friend class EFileSystem;
    public:
        FileEventNotifier();
        virtual ~FileEventNotifier();

    public:
        /* 콜백 파일 이벤트. */
        virtual void OnFileRegistered(const File::Path& path)                   {}
        virtual void OnFileUnregistered(const File::Path& path)                 {}
        virtual void OnFileModified(const Path& path)                           {}
        virtual void OnFileRemoved(const Path& path)                            {}
        virtual void OnFileRenamed(const Path& oldPath, const Path& newPath)    {}
        virtual void OnFileMoved(const Path& oldPath, const Path& newPath)      {}
        
        /* 프로젝트 세이브 요청을 처리할 동작을 구현 */
        virtual void OnRequestedSave()                                          {}
        virtual void OnPostRequestedSave()                                      {}
        /* 프로젝트 로드 요청을 처리할 동작을 구현 */
        virtual void OnRequestedLoad()                                          {}
        virtual void OnPostRequestedLoad()                                      {}
        /* 인스펙터에 출력 요청을 처리할 동작을 구현 */
        virtual void OnRequestedInspect(const File::Path& path)                 {}
        /* 파일을 여는 요청을 처리할 동작을 구현 */
        virtual void OnRequestedOpen(const File::Path& path)                    {}
        /* 파일을 복사했을 때 처리할 동작을 구현 */
        virtual void OnRequestedCopy(const File::Path& path)                    {}
        /* 파일을 붙여넣었을 때 처리할 동작을 구현 */
        virtual void OnRequestedPaste(const File::Path& path)                   {}

    public:
        bool IsTriggerExtension(const File::FString& ext);
        std::vector<FString> GetTriggerExtensions() const;

    private:
        std::unordered_set<FString> _triggerExtTable;
    };
}

