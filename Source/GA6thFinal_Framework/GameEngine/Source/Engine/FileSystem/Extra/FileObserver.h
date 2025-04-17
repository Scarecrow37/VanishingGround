#pragma once

namespace File
{
    struct FileEventData
    {
        Path      _lParam;
        Path      _rParam;
        EventType _eventType;
    };

    /*
    Observer클래스는 해당 파일 디렉터리를 관찰하여 발생하는 이벤트를 수집하여
    시스템에게 비동기적으로 이벤트를 보내줍니다.
    */
    class FileObserver
    {
        using CallBackFunc = std::function<void(const FileEventData&)>;
        using EventQueue   = std::deque<std::pair<Path, DWORD>>;
    public:
        FileObserver();
        ~FileObserver();
    public:
        bool Start(const File::Path& path, const CallBackFunc& callback);
        void Stop();
    private:
        void SetHandles();
        void SetThread();
    private:
        /* EventProcessing 메서드. 큐에 쌓인 이벤트를 하나씩 콜백해줍니다. */
        void        EventProcessingThread();
        EventType   CheckEvent(const EventQueue& q);
        void        ProcessUnKnown();
        void        ProcessAdded();
        void        ProcessRemoved();
        void        ProcessModified();
        void        ProcessRenamed();
        void        ProcessMoved();
        /* EventObserving 메서드. 디렉터리 이벤트를 관찰해 큐에 수집합니다. */
        void        EventObservingThread();
        bool        SetEventListener();
        void        RecieveFileEvents();

        void        EventDataToWStr(FileEventData& data, std::wstring& wstr);
        void        LastFileEventLog(FileEventData& event);
    private:
        Path            _path;
        CallBackFunc    _eventCallback;
        EventQueue      _recievedEventQueue;
        EventQueue      _sendEventQueue;
    private:
        BYTE            _recievedBytes[1024];   // 이벤트를 통해 받은 데이터
        DWORD           _bytesReturned;         // 받은 데이터의 크기

        HANDLE          _hDirectory;            // 파일 디렉터리에 접근하는 커널 핸들
        OVERLAPPED      _overlapped;            // 비동기 IO작업을 위한 오버랩 구조체
        std::thread     _eventProcessingThread; // 큐에 쌓인 이벤트를 콜백
        std::thread     _eventObservingThread;  // 디렉터리 이벤트를 관찰해 큐에 수집

        std::condition_variable _cv;
        std::mutex              _mutex;

        std::atomic<bool>       _request;       // 큐를 보낼 요청
        std::atomic<bool>       _isStart;       // Start 호출 여부
        std::atomic<bool>       _isObserving;   // 옵저버 스레드의 시작 여부
    private:
        constexpr static DWORD NOTIFY_FILTERS =
            FILE_NOTIFY_CHANGE_SECURITY | FILE_NOTIFY_CHANGE_CREATION |
            FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_ATTRIBUTES |
            FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_FILE_NAME;
    };
} // namespace File
