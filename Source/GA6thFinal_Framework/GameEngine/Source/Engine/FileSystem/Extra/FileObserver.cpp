#include "pch.h"
#include "FileObserver.h"

namespace File
{
    FileObserver::FileObserver() 
        : _isStart(false), _isObserving(false), _request(false),
          _recievedBytes({}), _bytesReturned(0), _hDirectory(NULL),
          _overlapped({})
    {
    }

    FileObserver::~FileObserver() 
    {
        Stop();
    }

    void FileObserver::SetCallbackFunc(const CallBackFunc& callback) 
    {
        _eventCallback = callback;
    }

    void FileObserver::SetObservingPath(const Path& path)
    {
        _path = path;
    }

    bool FileObserver::Start()
    {
        if (false == fs::exists(_path))
            return false;

        if (nullptr == _eventCallback)
            return false;

        if (true == _eventProcessingThread.joinable())
            _eventProcessingThread.join();

        if (true == _eventObservingThread.joinable())
            _eventObservingThread.join();

        if (false == _isStart)
        {
            _isStart = true;
            SetHandles();
            SetThread();
            OutputLog(L"FileObserver thread is Start");
            return true;
        }
        return false;
    }

    void FileObserver::Stop()
    {
        if (true == _isStart)
        {
            _isStart = false;
            _eventProcessingThread.join();
            _eventObservingThread.join();
            OutputLog(L"FileObserver thread is joined");
        }
    }

    void FileObserver::SetHandles()
    {
        if (true == std::filesystem::exists(_path) &&
            true == std::filesystem::is_directory(_path))
        {
            auto fileInfo = GetFileAttributesW(_path.c_str());
            if (INVALID_FILE_ATTRIBUTES == fileInfo)
            {
                throw std::system_error(GetLastError(), std::system_category());
            }

            _hDirectory = CreateFileW(
                _path.c_str(),                                  // 감시할 디렉토리 경로
                FILE_LIST_DIRECTORY,                            // 디렉토리 목록 조회 권한
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, // 공유 가능
                nullptr,                                        // 보안 속성 없음
                OPEN_EXISTING,                                  // 기존에 존재해야 함
                FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,  // 디렉토리 열기 허용 + 비동기 IO
                HANDLE(0));                                         // 템플릿 파일 없음

            if (INVALID_HANDLE_VALUE == _hDirectory)
            {
                ThrowSystemError();
            }
        }
    }

    void FileObserver::SetThread()
    {
        // 스레드 초기화
        _eventProcessingThread = std::thread([this]() {
            try
            {
                EventProcessingThread();
            }
            catch (...)
            {
            }
        });

        _eventObservingThread = std::thread([this]() {
            try
            {
                EventObservingThread();
            }
            catch (...)
            {
            }
        });
    }

    void FileObserver::EventProcessingThread()
    {
        while (true == _isStart)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            // 콜백 정보들이 없거나 destroy가 false일 경우 대기
            if (_recievedEventQueue.empty() && true == _isObserving)
            {
                // 옵저빙 스레드에서 notify_all이 호출되면 대기 해제
                _cv.wait(lock, [this] { return _recievedEventQueue.size() > 0 || false == _isObserving; });
            }
            // 콜백 정보를 이동 후 락 해제
            std::swap(_sendEventQueue, _recievedEventQueue);
            lock.unlock();

            CheckEvent();

            ProcessEvent();
        }
    }

    void FileObserver::CheckEvent()
    {
        for (auto& [action, info] : _sendEventQueue)
        {
            auto itr = _fileEventTable.find(info.FileId);
            if (itr == _fileEventTable.end())
            {
                _fileEventTable[info.FileId] = {"", "", Flag::FILE_EVENT_ACTION_UNKNOWN, info};
            }
        }

        while (false == _sendEventQueue.empty())
        {
            auto& [firstAction, firstInfo] = _sendEventQueue[0];

            if (_sendEventQueue.size() >= 2)
            {
                auto& [secondAction, secondInfo] = _sendEventQueue[1];

                bool checkFile = firstInfo.FileId == secondInfo.FileId;

                if (true == checkFile)
                {
                    // 이동 (현재 인덱스: removed, 다음 인덱스: added)
                    if (FILE_ACTION_REMOVED == firstAction && FILE_ACTION_ADDED == secondAction)
                    {
                        _fileEventTable[firstInfo.FileId].EventType |= Flag::FILE_EVENT_ACTION_MOVED;
                        _fileEventTable[firstInfo.FileId].LParam = firstInfo.FilePath.generic_wstring();
                        _fileEventTable[firstInfo.FileId].RParam = secondInfo.FilePath.generic_wstring();
                        _sendEventQueue.pop_front();
                        _sendEventQueue.pop_front();
                        continue;
                    }
                    else if (FILE_ACTION_RENAMED_OLD_NAME == firstAction &&
                             FILE_ACTION_RENAMED_NEW_NAME == secondAction)
                    {
                        _fileEventTable[firstInfo.FileId].EventType |= Flag::FILE_EVENT_ACTION_RENAMED;
                        _fileEventTable[firstInfo.FileId].LParam = firstInfo.FilePath.generic_wstring();
                        _fileEventTable[firstInfo.FileId].RParam = secondInfo.FilePath.generic_wstring();
                        _sendEventQueue.pop_front();
                        _sendEventQueue.pop_front();
                        continue;
                    }
                }
            }
            if (FILE_ACTION_ADDED == firstAction)
            {
                _fileEventTable[firstInfo.FileId].EventType |= Flag::FILE_EVENT_ACTION_ADDED;
                _fileEventTable[firstInfo.FileId].LParam = firstInfo.FilePath.generic_wstring();
                _sendEventQueue.pop_front();
                continue;
            }
            else if (FILE_ACTION_REMOVED == firstAction)
            {
                _fileEventTable[firstInfo.FileId].EventType |= Flag::FILE_EVENT_ACTION_REMOVED;
                _fileEventTable[firstInfo.FileId].LParam = firstInfo.FilePath.generic_wstring();
                _sendEventQueue.pop_front();
                continue;
            }
            else if (FILE_ACTION_MODIFIED == firstAction)
            {
                _fileEventTable[firstInfo.FileId].EventType |= Flag::FILE_EVENT_ACTION_MODIFIED;
                _fileEventTable[firstInfo.FileId].LParam = firstInfo.FilePath.generic_wstring();
                _sendEventQueue.pop_front();
                continue;
            }
            _sendEventQueue.pop_front();
        }
    }

    void FileObserver::ProcessEvent()
    {
        for (auto& [id, data] : _fileEventTable)
        {
            LastFileEventLog(data);
            if (nullptr != _eventCallback)
            {
                _eventCallback(data);
            }
        }
        _fileEventTable.clear();
    }

    void FileObserver::EventObservingThread()
    {
        DWORD bytesReturned = 0;

        ZeroMemory(&_overlapped, sizeof(_overlapped));
        _overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
        if (NULL == _overlapped.hEvent)
        {
            ThrowSystemError();
        }

        _isObserving = true;

        do
        {
            RecieveFileEvents();
            if (true == _request)
            {
                _request = false;
                _cv.notify_all();
            }
        } while (true == _isStart);

        CancelIoEx(_hDirectory, &_overlapped);
        GetOverlappedResult(_hDirectory, &_overlapped, &bytesReturned, TRUE);
        CloseHandle(_overlapped.hEvent);

        _isObserving = false;
        _cv.notify_all();
    }

    bool FileObserver::SetEventListener()
    {
        return ReadDirectoryChangesExW(
            _hDirectory,
            _recievedBytes,
            sizeof(_recievedBytes),
            TRUE, 
            NOTIFY_FILTERS, 
            &_bytesReturned,
            &_overlapped, 
            NULL,
            ReadDirectoryNotifyExtendedInformation 
        );
    }

    void FileObserver::RecieveFileEvents()
    {
        static bool listen = false;
        DWORD       bytes  = {};

        while (true)
        {
            // 파일 디렉터리 변경을 감지함
            if (false == listen)
            {
                if (true == SetEventListener())
                {
                    listen = true;
                }
                else
                {
                    listen = false;
                    break;
                }
            }

            // 변경이 감지되어 이벤트를 처리 중이면
            if (TRUE == GetOverlappedResultEx(_hDirectory, &_overlapped, &bytes, 100, FALSE))
            {
                // IO작업이 진행중이면 기다림
                if (ERROR_IO_INCOMPLETE == GetLastError())
                {
                    continue;
                }
                else
                {
                    listen = false;
                    // 변경된 데이터에 대한 처리
                    FILE_NOTIFY_EXTENDED_INFORMATION* fileInfo =
                        reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION*>(&_recievedBytes[0]);

                    while (true)
                    {
                        _request = true;

                        std::wstring filename(fileInfo->FileName, fileInfo->FileNameLength / sizeof(WCHAR));

                        FileInformation info = {filename,
                                                fileInfo->CreationTime.QuadPart,
                                                fileInfo->LastAccessTime.QuadPart,
                                                fileInfo->LastChangeTime.QuadPart,
                                                fileInfo->LastAccessTime.QuadPart,
                                                fileInfo->FileId.QuadPart,
                                                fileInfo->ParentFileId.QuadPart};
                        {
                            std::lock_guard<std::mutex> lock(_mutex);
                            _recievedEventQueue.push_back(std::make_pair(fileInfo->Action, info));
                        }

                        if (0 == fileInfo->NextEntryOffset)
                            break;

                        fileInfo = reinterpret_cast<FILE_NOTIFY_EXTENDED_INFORMATION*>(
                            reinterpret_cast<BYTE*>(fileInfo) + fileInfo->NextEntryOffset);
                    }
                }
            }
            // 변경이 없다
            else
            {
                return;
            }
        }
    }
   
    void FileObserver::EventDataToWStr(FileEventData& data, std::wstring& wstr)
    {
        const auto& [lParam, rParam, event, info] = data;
        wstr += L"(LParam: ";
        wstr += lParam.wstring();
        wstr += L", RParam: ";
        wstr += rParam.wstring();
        wstr += L", EventType: ";
        if (event & Flag::FILE_EVENT_ACTION_RENAMED)
        {
            wstr += L"[FILE_EVENT_ACTION_RENAMED]";
        }
        if (event & Flag::FILE_EVENT_ACTION_MOVED)
        {
            wstr += L"[FILE_EVENT_ACTION_MOVED]";
        }
        if (event & Flag::FILE_EVENT_ACTION_ADDED)
        {
            wstr += L"[FILE_EVENT_ACTION_ADDED]";
        }
        if (event & Flag::FILE_EVENT_ACTION_REMOVED)
        {
            wstr += L"[FILE_EVENT_ACTION_REMOVED]";
        }
        if (event & Flag::FILE_EVENT_ACTION_MODIFIED)
        {
            wstr += L"[FILE_EVENT_ACTION_MODIFIED]";
        }
        wstr += L", FileId: ";
        wstr += std::to_wstring(info.FileId);
        wstr += L")";
    }

    void FileObserver::LastFileEventLog(FileEventData& event) 
    {
#ifdef _DEBUG
        if (UmFileSystem.GetDebugLevel() >= 1)
        {
            std::wstring wstr;
            EventDataToWStr(event, wstr);
            OutputLog(L"FileObserver send file event " + wstr);
        }
#endif
    }
} // namespace File