#include "pch.h"
#include "FileSystemModule.h"

FileSystemModule::FileSystemModule()
    : _observer(nullptr)
{
}

FileSystemModule::~FileSystemModule() 
{
}

void FileSystemModule::PreInitialize()
{
}

void FileSystemModule::ModuleInitialize()
{
    File::Path filename  = L"filesystem.setting";
    File::Path directory = PROJECT_SETTING_PATH;
    UmFileSystem.LoadSetting(directory / filename);

    UmFileSystem.RegisterFileEventNotifier(new SampleNotifier,
                                           {".txt", ".png", ".dds"});

    _observer = new File::FileObserver();
    _observer->Start(UmFileSystem.GetRootPath(),
                     [this](const Event& event) { 
        RecieveFileEvent(event);
    });

    UmFileSystem.Reload();
}

void FileSystemModule::PreUnInitialize() 
{
}

void FileSystemModule::ModuleUnInitialize() 
{
    if (nullptr != _observer)
    {
        _observer->Stop();
        delete _observer;
        _observer = nullptr;
    }
    UmFileSystem.Clear();
}

void FileSystemModule::Update() 
{
    auto& filesystem = UmFileSystem;
    DispatchFileEvent();
}

void FileSystemModule::RecieveFileEvent(const Event& data)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _eventQueue.push(data);
}

void FileSystemModule::DispatchFileEvent()
{
    if (true == _eventQueue.empty())
        return;

    std::lock_guard<std::mutex> lock(_mutex);

    while (false == _eventQueue.empty())
    {
        const auto& [lParam, rParam, eventType] = _eventQueue.front();

        const File::Path& rootPath = UmFileSystem.GetRootPath();

        File::Path lp = (rootPath / lParam).generic_string();
        File::Path rp = (rootPath / rParam).generic_string();

        if (true == UmFileSystem.IsValidExtension(lp.extension()))
        {
            std::unordered_set<File::FileEventNotifier*> notifiers;

            switch (eventType)
            {
            case File::EventType::ADDED:
            {
                UmFileSystem.AddedFile(lp);
                break;
            }
            case File::EventType::REMOVED:
            {
                UmFileSystem.RemovedFile(lp);
                break;
            }
            case File::EventType::MODIFIED:
            {
                UmFileSystem.ModifiedFile(lp);
                break;
            }
            case File::EventType::RENAMED:
            {
                UmFileSystem.MovedFile(lp, rp);
                break;
            }
            case File::EventType::MOVED:
            {
                UmFileSystem.MovedFile(lp, rp);
                break;
            }
            default:
                break;
            }
        }
        _eventQueue.pop();
    }
}

void SampleNotifier::OnRequestedOpen(const File::Path& path) 
{
    File::OpenFile(path);
}

void SampleNotifier::OnRequestedCopy(const File::Path& path) 
{
    UmEngineLogger.Log(1, "Copy File");
}

void SampleNotifier::OnRequestedPaste(const File::Path& path) 
{
    UmEngineLogger.Log(1, "Paste File");
}
