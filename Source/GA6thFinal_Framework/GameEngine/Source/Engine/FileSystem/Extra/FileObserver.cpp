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

    bool FileObserver::Start(const std::wstring& path,
                             const CallBackFunc& callback)
    {
        if (nullptr == callback)
            return false;

        if (false == _isStart)
        {
            _isStart       = true;
            _path          = path;
            _eventCallback = callback;
            SetHandles();
            SetThread();
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
        if (std::filesystem::exists(_path) &&
            std::filesystem::is_directory(_path))
        {
            auto fileInfo = GetFileAttributesW(_path.c_str());
            if (fileInfo == INVALID_FILE_ATTRIBUTES)
            {
                throw std::system_error(GetLastError(), std::system_category());
            }

            _hDirectory = CreateFileW(
                _path.c_str(),       // 감시할 디렉토리 경로
                FILE_LIST_DIRECTORY, // 디렉토리 목록 조회 권한
                FILE_SHARE_READ | FILE_SHARE_WRITE |
                    FILE_SHARE_DELETE, // 공유 가능
                nullptr,               // 보안 속성 없음
                OPEN_EXISTING,         // 기존에 존재해야 함
                FILE_FLAG_BACKUP_SEMANTICS |
                    FILE_FLAG_OVERLAPPED, // 디렉토리 열기 허용 + 비동기 IO
                HANDLE(0));               // 템플릿 파일 없음

            if (_hDirectory == INVALID_HANDLE_VALUE)
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
                _cv.wait(lock, [this] {
                    return _recievedEventQueue.size() > 0 ||
                           false == _isObserving;
                });
            }
            // 콜백 정보를 이동 후 락 해제
            std::swap(_sendEventQueue, _recievedEventQueue);
            lock.unlock();

            while (false == _sendEventQueue.empty())
            {
                switch (CheckEvent(_sendEventQueue))
                {
                case EventType::UNKNOWN:
                    ProcessUnKnown();
                    break;
                case EventType::ADDED:
                    ProcessAdded();
                    break;
                case EventType::REMOVED:
                    ProcessRemoved();
                    break;
                case EventType::MODIFIED:
                    ProcessModified();
                    break;
                case EventType::RENAMED:
                    ProcessRenamed();
                    break;
                case EventType::MOVED:
                    ProcessMoved();
                    break;
                default:
                    ProcessUnKnown();
                    break;
                }
            }
        }
    }

    void FileObserver::EventObservingThread()
    {
        DWORD             bytesReturned = 0;

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
        /*
        ReadDirectoryChangesExW는 문서가 너무 없어서 그냥 안전하게 기존 래거시 함수 사용.
        */
        return ReadDirectoryChangesW(_hDirectory, _recievedBytes, sizeof(_recievedBytes),
                                     TRUE, NOTIFY_FILTERS, &_bytesReturned,
                                     &_overlapped, NULL);
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
            if (TRUE == GetOverlappedResultEx(_hDirectory, &_overlapped, &bytes, 0, FALSE))
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
                    FILE_NOTIFY_INFORMATION* fileInfo =
                        reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                            &_recievedBytes[0]);

                    while (true)
                    {
                        std::wstring filename(fileInfo->FileName,
                                              fileInfo->FileNameLength /
                                                  sizeof(WCHAR));

                        if (false == filename.empty())
                        {
                            std::lock_guard<std::mutex> lock(_mutex);
                            _recievedEventQueue.push_back(
                                std::make_pair(filename, fileInfo->Action));
                        }

                        if (0 == fileInfo->NextEntryOffset)
                            break;

                        fileInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                            reinterpret_cast<BYTE*>(fileInfo) +
                            fileInfo->NextEntryOffset);
                    }

                    std::this_thread::sleep_for(
                        std::chrono::milliseconds(3)); 

                    _request = true;
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
        switch (data._eventType)
        {
        case File::EventType::UNKNOWN: {
            wstr = L"(EventType: UNKNOWN";
            wstr += L", lParam: ";
            wstr += data._lParam;
            wstr += L", rParam: ";
            wstr += data._rParam;
            wstr += L")";
            break;
        }
        case File::EventType::ADDED: {
            wstr = L"(EventType: ADDED";
            wstr += L", lParam: ";
            wstr += data._lParam;
            wstr += L")";
            break;
        }
        case File::EventType::REMOVED: {
            wstr = L"(EventType: REMOVED";
            wstr += L", lParam: ";
            wstr += data._lParam;
            wstr += L")";
            break;
        }
        case File::EventType::MODIFIED: {
            wstr = L"(EventType: MODIFIED";
            wstr += L", lParam: ";
            wstr += data._lParam;
            wstr += L")";
            break;
        }
        case File::EventType::RENAMED: {
            wstr = L"(EventType: RENAMED";
            wstr += L", lParam: ";
            wstr += data._lParam;
            wstr += L", rParam: ";
            wstr += data._rParam;
            wstr += L")";
            break;
        }
        case File::EventType::MOVED: {
            wstr = L"(EventType: MOVED";
            wstr += L", lParam: ";
            wstr += data._lParam;
            wstr += L", rParam: ";
            wstr += data._rParam;
            wstr += L")";
            break;
        }
        default:
            break;
        }
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

    /*
    같은 디렉터리 내 이름 변경					FILE_ACTION_RENAMED_OLD_NAME +
    FILE_ACTION_RENAMED_NEW_NAME 다른 디렉터리로 이동
    경우에 따라 DELETE + CREATE, 또는 rename 쌍 다른 드라이브로 이동
    실제로는 복사 → 삭제이므로, CREATE + DELETE 외부에서 파일 던지기 (예:
    탐색기에서 드래그)	CREATE만 감지됨 (삭제는 외부 시스템 이벤트라 감지 못함)
    */
    EventType FileObserver::CheckEvent(const EventQueue& q)
    {
        if (q.empty())
            return EventType::UNKNOWN;

        auto& [firstPath, firstEvent] = q[0];

        if (_sendEventQueue.size() >= 2)
        {
            auto& [secondPath, secondEvent] = q[1];

            bool checkFile = firstPath.filename() == secondPath.filename();

            if (checkFile)
            {
                // 이동 (현재 인덱스: removed, 다음 인덱스: added, 각 파일명
                // 동일해야함)
                if (FILE_ACTION_REMOVED == firstEvent &&
                    FILE_ACTION_ADDED == secondEvent)
                    return EventType::MOVED;
            }
            else
            {
                // 이름 변경 (현재 인덱스: renamed_old, 다음 인덱스:
                // renamed_new)
                if (FILE_ACTION_RENAMED_OLD_NAME == firstEvent &&
                    FILE_ACTION_RENAMED_NEW_NAME == secondEvent)
                    return EventType::RENAMED;
            }
        }

        if (FILE_ACTION_ADDED == firstEvent)
            return EventType::ADDED;

        if (FILE_ACTION_REMOVED == firstEvent)
            return EventType::REMOVED;

        if (FILE_ACTION_MODIFIED == firstEvent)
            return EventType::MODIFIED;

        return EventType::UNKNOWN;
    }

    void FileObserver::ProcessUnKnown()
    {
        FileEventData data = {"", "", EventType::UNKNOWN};
        _sendEventQueue.pop_front();
    }

    void FileObserver::ProcessAdded()
    {
        const auto& [path, event] = _sendEventQueue[0];

        FileEventData data = {path.generic_wstring(), "", EventType::ADDED};
        LastFileEventLog(data);

        _eventCallback(data);
        _sendEventQueue.pop_front();
    }
    void FileObserver::ProcessRemoved()
    {
        const auto& [path, event] = _sendEventQueue[0];

        FileEventData data = {path.generic_wstring(), "", EventType::REMOVED};
        LastFileEventLog(data);

        _eventCallback(data);
        _sendEventQueue.pop_front();
    }
    void FileObserver::ProcessModified()
    {
        const auto& [path, event] = _sendEventQueue[0];

        FileEventData data = {path.generic_wstring(), "", EventType::MODIFIED};
        LastFileEventLog(data);

        _eventCallback(data);
        _sendEventQueue.pop_front();
    }
    void FileObserver::ProcessRenamed()
    {
        const auto& [firstPath, firstEvent]   = _sendEventQueue[0];
        const auto& [secondPath, secondEvent] = _sendEventQueue[1];

        // lParam: 이전 이름
        // rParam: 새 이름
        FileEventData data = {firstPath.generic_wstring(),
                              secondPath.generic_wstring(), EventType::RENAMED};
        LastFileEventLog(data);

        _eventCallback(data);
        _sendEventQueue.pop_front();
        _sendEventQueue.pop_front();
    }
    void FileObserver::ProcessMoved()
    {
        const auto& [firstPath, firstEvent]   = _sendEventQueue[0];
        const auto& [secondPath, secondEvent] = _sendEventQueue[1];

        // lParam: 이전 경로
        // rParam: 새 경로
        FileEventData data = {firstPath.generic_wstring(),
                              secondPath.generic_wstring(), EventType::MOVED};
        LastFileEventLog(data);

        _eventCallback(data);
        _sendEventQueue.pop_front();
        _sendEventQueue.pop_front();
    }
} // namespace File