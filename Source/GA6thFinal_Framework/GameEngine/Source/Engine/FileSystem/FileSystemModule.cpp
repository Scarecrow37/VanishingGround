#include "pch.h"
#include "FileSystemModule.h"

using namespace u8_literals;

FileSystemModule::FileSystemModule()
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
    HWND hwnd = UmApplication.GetHwnd();
    DragAcceptFiles(hwnd, TRUE);

    const MessageHandler msgHandler(FileSystemWinProc, 0);

    UmApplication.AddMessageHandler(msgHandler);
    UmFileSystem.ObserverSetUp([this](const Event& event) { RecieveFileEvent(event); });
    auto accessExt = {".txt", ".png", ".dds"};
    UmFileSystem.RegisterFileEventNotifier(new SampleNotifier, accessExt);
}

void FileSystemModule::PreUnInitialize() 
{
    UmFileSystem.ObserverShutDown();
    UmFileSystem.Clear();
}

void FileSystemModule::ModuleUnInitialize() 
{
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

        const File::Path& rootPath = UmFileSystem.GetRootPath();

        File::Path lp = (rootPath / lParam).generic_string();
        File::Path rp = (rootPath / rParam).generic_string();

        std::unordered_set<File::FileEventNotifier*> notifiers;

        switch (eventType)
        {
        case File::EventType::ADDED: {
            UmFileSystem.RegisterContext(lp);
            break;
        }
        case File::EventType::REMOVED: {
            UmFileSystem.ProcessRemovedFile(lp);
            break;
        }
        case File::EventType::MODIFIED: {
            UmFileSystem.ProcessModifiedFile(lp);
            break;
        }
        case File::EventType::RENAMED: {
            UmFileSystem.ProcessMovedFile(lp, rp);
            break;
        }
        case File::EventType::MOVED: {
            UmFileSystem.ProcessMovedFile(lp, rp);
            break;
        }
        default:
            break;
        }

        _eventQueue.pop();
    }
}

void FileSystemModule::ProcessDropFile(const HDROP hDrop)
{
    // 드롭된 파일의 개수
    UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

    for (size_t i = 0; i < fileCount; ++i)
    {
        // 각 파일의 절대경로를 얻음
        wchar_t targetPath[MAX_PATH];
        DragQueryFile(hDrop, i, targetPath, MAX_PATH);

        File::Path project = targetPath;
        File::Path extension = project.extension();
        if (File::PROJECT_EXTENSION == extension)
        {
            UmFileSystem.SaveProjectWithMessageBox();
            UmFileSystem.LoadProjectWithMessageBox(targetPath);
        }
    }
    // 메모리 해제
    DragFinish(hDrop);
}

bool FileSystemModule::FileSystemWinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_CLOSE: 
        {
            UmFileSystem.SaveProjectWithMessageBox();
            // 이후 처리 동작은 Application이 호출한다.
            break;
        }
        case WM_DROPFILES:
        {
            ProcessDropFile((HDROP)wParam);
            break;
        }
    }
    return false;
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

void SampleNotifier::OnRequestedSave() 
{
    UmLogger.Log(1, "OnRequestedSave");
}

void SampleNotifier::OnRequestedLoad() 
{
    UmLogger.Log(1, "OnRequestedLoad");
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
