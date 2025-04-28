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

    UmFileSystem.RegisterFileEventNotifier(new SampleNotifier, {".txt", ".png", ".dds", ".fbx"});

    _observer = new File::FileObserver();
    _observer->Start(UmFileSystem.GetRootPath(),
                     [this](const Event& event) { 
        RecieveFileEvent(event);
    });

    UmFileSystem.ReadDirectory();
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
                UmFileSystem.RegisterContext(lp);
                break;
            }
            case File::EventType::REMOVED:
            {
                UmFileSystem.ProcessRemovedFile(lp);
                break;
            }
            case File::EventType::MODIFIED:
            {
                UmFileSystem.ProcessModifiedFile(lp);
                break;
            }
            case File::EventType::RENAMED:
            {
                UmFileSystem.ProcessMovedFile(lp, rp);
                break;
            }
            case File::EventType::MOVED:
            {
                UmFileSystem.ProcessMovedFile(lp, rp);
                break;
            }
            default:
                break;
            }
        }
        _eventQueue.pop();
    }
}

void SampleNotifier::OnFileRegistered(const File::Path& path) 
{
    std::wstring log = L"Register File: " + path.generic_wstring();
    File::OutputLog(log);
}

void SampleNotifier::OnFileUnregistered(const File::Path& path) 
{
    std::wstring log = L"Unregister File: " + path.generic_wstring();
    File::OutputLog(log);
}

void SampleNotifier::OnFileModified(const File::Path& path) 
{
    std::wstring log = L"Modified File: " + path.generic_wstring();
    File::OutputLog(log);
}

void SampleNotifier::OnFileRemoved(const File::Path& path) 
{
    std::wstring log = L"Removed File: " + path.generic_wstring();
    File::OutputLog(log);
}

void SampleNotifier::OnFileRenamed(const File::Path& oldPath, const File::Path& newPath) 
{
    std::wstring log = L"Renamed File: " + oldPath.generic_wstring() + L" to " + newPath.generic_wstring();
    File::OutputLog(log);
}

void SampleNotifier::OnFileMoved(const File::Path& oldPath, const File::Path& newPath) 
{
    std::wstring log = L"Moved File: " + oldPath.generic_wstring() + L" to " + newPath.generic_wstring();
    File::OutputLog(log);
}

void SampleNotifier::OnRequestedInspect(const File::Path& path) 
{
    ImGui::Text("TestInspect");
}

void SampleNotifier::OnRequestedOpen(const File::Path& path)
{
    File::OpenFile(path);
}

void SampleNotifier::OnRequestedCopy(const File::Path& path) 
{
    UmLogger.Log(1, "Copy File");
}

void SampleNotifier::OnRequestedPaste(const File::Path& path) 
{
    UmLogger.Log(1, "Paste File");
}
