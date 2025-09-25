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
    // FileSystemModule is Only Call Editor Mode
    if constexpr (true == IS_EDITOR)
    {
        HWND hwnd = UmApplication.GetHwnd();
        DragAcceptFiles(hwnd, TRUE);
        // 파일 시스템 관련 메세지는 시스템을 정리하거나 로드하는 과정이므로 항상 최후순위로 미룬다.
        const MessageHandler msgHandler(FileSystemWinProc, 999);
        UmApplication.AddMessageHandler(msgHandler);

        UmFileSystem.ObserverSetUp([this](const Event& event) { RecieveFileEvent(event); });
    }

    auto accessExt = {".UmAnimEvent", ".UmQTETrack"};
    UmFileSystem.RegisterFileEventSubscriber(this, accessExt);

    try
    {
        _spriteFontImporter.Initialize();
    }
    catch (const std::exception& e)
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, e.what());
    }
}

void FileSystemModule::PreUnInitialize() 
{
    if constexpr (true == IS_EDITOR)
    {
        UmFileSystem.ObserverShutDown();
    }
    UmFileSystem.Clear();
}

void FileSystemModule::ModuleUnInitialize() 
{
}

void FileSystemModule::OnRequestedSave() 
{
    auto& path = UmFileSystem.GetProjectSettingPath();
    auto name = File::PROJECT_SETTING_FILENAME;
    UmFileSystem.SaveSetting(path / name);
}

void FileSystemModule::OnRequestedLoad() 
{
    auto& path = UmFileSystem.GetProjectSettingPath();
    auto  name = File::PROJECT_SETTING_FILENAME;
    UmFileSystem.LoadSetting(path / name);
}

void FileSystemModule::OnRequestedDragDrop(const File::Path& path) 
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
        const Event& eventPacket = _eventQueue.front();
        const File::Path& rootPath = UmFileSystem.GetRootPath();
        const auto& [lParamTable, rParamTable, event, info] = eventPacket;

        if (event & File::Flag::FILE_EVENT_ACTION_RENAMED)
        {
            File::Flag::EventAction eventType = File::Flag::FILE_EVENT_ACTION_RENAMED;
            File::Path lp = (rootPath / eventPacket.GetLParam(eventType)).generic_string();
            File::Path rp = (rootPath / eventPacket.GetRParam(eventType)).generic_string();
            UmFileSystem.ProcessMovedFile(lp, rp);
        }
        else if (event & File::Flag::FILE_EVENT_ACTION_MOVED)
        {
            File::Flag::EventAction eventType = File::Flag::FILE_EVENT_ACTION_MOVED;
            File::Path lp = (rootPath / eventPacket.GetLParam(eventType)).generic_string();
            File::Path rp = (rootPath / eventPacket.GetRParam(eventType)).generic_string();
            UmFileSystem.ProcessMovedFile(lp, rp);
        }
        else if (event & File::Flag::FILE_EVENT_ACTION_ADDED)
        {
            File::Flag::EventAction eventType = File::Flag::FILE_EVENT_ACTION_ADDED;
            File::Path lp = (rootPath / eventPacket.GetLParam(eventType)).generic_string();
            UmFileSystem.RegisterContext(lp);
        }
        else if (event & File::Flag::FILE_EVENT_ACTION_REMOVED)
        {
            File::Flag::EventAction eventType = File::Flag::FILE_EVENT_ACTION_REMOVED;
            File::Path lp = (rootPath / eventPacket.GetLParam(eventType)).generic_string();
            UmFileSystem.ProcessRemovedFile(lp);
        }
        else if (event & File::Flag::FILE_EVENT_ACTION_MODIFIED)
        {
            File::Flag::EventAction eventType = File::Flag::FILE_EVENT_ACTION_MODIFIED;
            File::Path lp = (rootPath / eventPacket.GetLParam(eventType)).generic_string();
            UmFileSystem.ProcessModifiedFile(lp);
        }
        else if (event == File::Flag::FILE_EVENT_ACTION_UNKNOWN)
        {
            assert(false && L"Unknown File Event");
        }
        _eventQueue.pop();
    }
}

void FileSystemModule::ProcessDropFile(const HDROP hDrop)
{
    // 드롭된 파일의 개수
    UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

    for (UINT i = 0; i < fileCount; ++i)
    {
        // 각 파일의 절대경로를 얻음
        wchar_t targetPath[MAX_PATH];
        DragQueryFile(hDrop, i, targetPath, MAX_PATH);
        File::Path path = targetPath;
        UmFileSystem.RequestDragDropFile(path);

        File::Path extension = path.extension();
        if (false == UmCore->IsPlay() && File::PROJECT_EXTENSION == extension)
        {
            UmFileSystem.SaveProjectWithMessageBox();
            UmFileSystem.LoadProjectWithMessageBox(path);
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
            if (true == UmFileSystem.IsLoadedProject())
            {
                if (IDCANCEL == UmFileSystem.SaveProjectWithMessageBox())
                {
                    return true;
                }
            }
            // 이후 처리 동작은 Application이 호출한다.
            break;
        }
        case WM_DROPFILES:
        {
            ProcessDropFile((HDROP)wParam);
            return true;
            break;
        }
    }
    return false;
}