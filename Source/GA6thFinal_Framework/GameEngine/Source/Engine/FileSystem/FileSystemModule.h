#pragma once

namespace File
{
    class FileEventNotifier;
    class FileObserver;
    class Context;
    class FileContext;
    class FolderContext;
    struct FileEventData;
} // namespace File

class FileSystemModule;
class SampleFileEventNotifier;

/*
FileSystemModule은 파일 시스템을 셋업하고, 비동기적으로 받은 이벤트를
엔진에 동기적으로 전달하는 모듈이다.
*/
class FileSystemModule : public IAppModule
{
    using Event = File::FileEventData;
    using NotifierSet = std::unordered_set<File::FileEventNotifier*>;

public:
    FileSystemModule();
    ~FileSystemModule();
public:
    void PreInitialize() override;
    void ModuleInitialize() override;

    void PreUnInitialize() override;
    void ModuleUnInitialize() override;

public:
    bool SaveSetting(const File::Path& path);
    bool LoadSetting(const File::Path& path);

    void Update();

private:
    void RecieveFileEvent(const Event& data);
    void DispatchFileEvent();

private:
    File::FileObserver* _observer; // 파일 디렉터리 이벤트를 감시하는 옵저버.

    std::mutex          _mutex;
    std::queue<Event>   _eventQueue; // 이벤트 큐
};

/* 테스트용 클래스. 추후 제거해야함 */
class SampleNotifier : public File::FileEventNotifier
{
public:
    SampleNotifier() {}
    virtual ~SampleNotifier() {}

public:
    virtual void OnFileRegistered(const File::Path& path) override;
    virtual void OnFileUnregistered(const File::Path& path) override;
    virtual void OnFileModified(const File::Path& path) override;
    virtual void OnFileRemoved(const File::Path& path) override;
    virtual void OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) override;
    virtual void OnFileMoved(const File::Path& oldPath, const File::Path& newPath) override;

    virtual void OnRequestedInspect(const File::Path& path) override;
    virtual void OnRequestedOpen(const File::Path& path) override;
    virtual void OnRequestedCopy(const File::Path& path) override;
    virtual void OnRequestedPaste(const File::Path& path) override;
};