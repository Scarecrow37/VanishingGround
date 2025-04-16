#include "pch.h"
#include "FileSystemModule.h"

FileSystemModule* Global::fileSystem = nullptr;

FileSystemModule::FileSystemModule()
    : _observer(nullptr)
{
    Global::fileSystem = this;
}

FileSystemModule::~FileSystemModule() 
{
    Global::fileSystem = nullptr;
}

void FileSystemModule::PreInitialize()
{
}

void FileSystemModule::ModuleInitialize()
{
    UmFileSystem.RegisterFileEventNotifier(new SampleNotifier,
                                           {".txt", ".png", ".dds"});
    UmFileSystem.SetDebugLevel(1);

    UmFileSystem.ReadDirectory(_rootPath);

    _observer = new File::FileObserver();
    _observer->Start(_rootPath, [this](const Event& event) { 
        RecieveFileEvent(event);
    });
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

        File::Path lp = (_rootPath / lParam).generic_string();
        File::Path rp = (_rootPath / rParam).generic_string();

        if (true == UmFileSystem.IsValidExtension(lp.extension()))
        {
            std::unordered_set<File::FileEventNotifier*> notifiers;

            if (std::filesystem::is_regular_file(lp))
            {
                notifiers = UmFileSystem.GetNotifiers(lp.extension());
            }

            switch (eventType)
            {
            case File::EventType::ADDED:
            {
                UmFileSystem.AddedFile(lp);
                for (auto& notifier : notifiers)
                    notifier->OnFileAdded(lp);
                break;
            }
            case File::EventType::REMOVED:
            {
                UmFileSystem.RemovedFile(lp);
                for (auto& notifier : notifiers)
                    notifier->OnFileRemoved(lp);
                break;
            }
            case File::EventType::MODIFIED:
            {
                UmFileSystem.ModifiedFile(lp);
                for (auto& notifier : notifiers)
                    notifier->OnFileModified(lp);
                break;
            }
            case File::EventType::RENAMED:
            {
                UmFileSystem.MovedFile(lp, rp);
                for (auto& notifier : notifiers)
                    notifier->OnFileRenamed(lp, rp);
                break;
            }
            case File::EventType::MOVED:
            {
                UmFileSystem.MovedFile(lp, rp);
                for (auto& notifier : notifiers)
                    notifier->OnFileMoved(lp, rp);
                break;
            }
            default:
                break;
            }
        }
        _eventQueue.pop();
    }
}