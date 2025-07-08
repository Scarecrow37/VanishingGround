#pragma once

namespace File
{
    class FileEventSubscriber;
    class FileEventObserver;
    class Context;
    class FileContext;
    class FolderContext;
    struct FileEventData;
} // namespace File

/*
FileSystemModule은 파일 시스템을 셋업하고, Observer를 통해 비동기적으로 받은 이벤트를
엔진에 동기적으로 전달하는 모듈이다.
Editor모드에서만 생성 및 사용하며, Game빌드 시에는 사용하지 않는다.
*/
class FileSystemModule 
    : public IAppModule
    , public File::FileEventSubscriber
{
    using Event = File::FileEventData;
    using NotifierSet = std::unordered_set<File::FileEventSubscriber*>;

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