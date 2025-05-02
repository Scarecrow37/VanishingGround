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
class FileSystemModule 
    : public IAppModule
    , public File::FileEventNotifier
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

    void OnRequestedSave() override;
    void OnRequestedLoad() override;
public:

    void Update();

private:
    void RecieveFileEvent(const Event& data);
    void DispatchFileEvent();

    static void ProcessDropFile(const HDROP hDrop);
    static bool FileSystemWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    std::mutex          _mutex;
    std::queue<Event>   _eventQueue; // 이벤트 큐
};