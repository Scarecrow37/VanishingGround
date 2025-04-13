#include "pch.h"
#include "FileSystemModule.h"
#include "../FileSystem/framework.h"

FileSystemModule* Global::fileSystem = nullptr;

FileSystemModule::FileSystemModule()
    : _observer(nullptr)
{
    Global::fileSystem = this;
    sampleNotifier     = new SampleFileEventNotifier({".txt", ".png", ".dds"});

    File::FileSystem::RegisterFileEventNotifier(sampleNotifier);
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
    File::FileSystem::ReadDirectory(_rootPath);

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
    File::FileSystem::Clear();
}

void FileSystemModule::Update() 
{
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

        if (true == File::FileSystem::IsValidExtension(lp.extension()))
        {
            std::unordered_set<File::FileEventNotifier*> notifiers;

            if (std::filesystem::is_regular_file(lp))
            {
                notifiers = File::FileSystem::GetNotifiers(lp.extension());
            }

            switch (eventType)
            {
            case File::EventType::ADDED:
            {
                File::FileSystem::AddedFile(lp);
                for (auto& notifier : notifiers)
                    notifier->OnFileAdded(lp);
                break;
            }
            case File::EventType::REMOVED:
            {
                File::FileSystem::RemovedFile(lp);
                for (auto& notifier : notifiers)
                    notifier->OnFileRemoved(lp);
                break;
            }
            case File::EventType::MODIFIED:
            {
                File::FileSystem::ModifiedFile(lp);
                for (auto& notifier : notifiers)
                    notifier->OnFileModified(lp);
                break;
            }
            case File::EventType::RENAMED:
            {
                File::FileSystem::MovedFile(lp, rp);
                for (auto& notifier : notifiers)
                    notifier->OnFileRenamed(lp, rp);
                break;
            }
            case File::EventType::MOVED:
            {
                File::FileSystem::MovedFile(lp, rp);
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

void SampleFileEventNotifier::OnFileAdded(const File::Path& path) {}

void SampleFileEventNotifier::OnFileModified(const File::Path& path) {}

void SampleFileEventNotifier::OnFileRemoved(const File::Path& path) {}

void SampleFileEventNotifier::OnFileRenamed(const File::Path& oldPath,
                                            const File::Path& newPath)
{
}

void SampleFileEventNotifier::OnFileMoved(const File::Path& oldPath,
                                          const File::Path& newPath)
{
}
