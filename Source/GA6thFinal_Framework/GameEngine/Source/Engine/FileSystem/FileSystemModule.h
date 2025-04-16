#pragma once

namespace File
{
    class FileEventNotifier;
    class FileObserver;
    class Context;
    class FileContext;
    class ForderContext;
    struct FileEventData;
} // namespace File

class FileSystemModule;
class SampleFileEventNotifier;

namespace Global
{
    extern FileSystemModule* fileSystem;
}

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
    void Update();

public:
    inline const auto& GetRootPath() 
    {
        return _rootPath;
    }

private:
    void RecieveFileEvent(const Event& data);
    void DispatchFileEvent();

private:
    // 관리할 루트 패스
    const File::Path   _rootPath = "Assets";

    File::FileObserver* _observer; // 파일 디렉터리 이벤트를 감시하는 옵저버.

    std::mutex          _mutex;
    std::queue<Event>   _eventQueue; // 이벤트 큐

    SampleFileEventNotifier* sampleNotifier;
};