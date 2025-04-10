#pragma once

namespace File
{
    class FileObserver;
    class FileContext;

    class FileSystem
    {
    public:
        FileSystem()                                = default;
        ~FileSystem()                               = default;
        FileSystem(const FileSystem&)               = delete;
        FileSystem& operator=(const FileSystem&)    = delete;
        FileSystem(FileSystem&&)                    = delete;
        FileSystem& operator=(FileSystem&&)         = delete;
    public:
        void Initialize();
        void DeInitialize();
    public:
        void Update();
    public:
        /* 폴더를 순회하며 파일 탐색 */
        void ReadDiectory(const Path& path);
    public:

        /* 파일 이벤트를 받아옴 */
        void RecieveFileEvents(const FileEventData& data);

        /* 파일 이벤트 처리 */
        void ProcessEventQueue(); 

        

        const Path& GetRootPath() const;
    private:
        const Path _rootPath = "Assets";

        FileObserver* _observer; // 파일 디렉터리 이벤트를 감시하는 옵저버.

        std::mutex _mutex;
        std::vector<FileEventData> _eventQueue; // 이벤트 큐

       
    };

    class IDMapper
    {
        friend class FileSystem;

    public:
        static const File::Path& GetPathFromGuid(const File::Guid& guid);
        static const File::Guid& GetGuidFromPath(const File::Path& path);
        static FileContext* GetFileContext(const File::Guid& guid);
        static FileContext* GetFileContext(const File::Path& path);

        static bool IsVaildGuid(const File::Guid& guid);
        static bool IsVaildExtension(const File::Path& path);

    private:
        static void Clear();
        static void AddedFile(const File::Path& path);
        static void RemovedFile(const File::Path& path);
        static void ModifiedFile(const File::Path& path);
        static void MovedFile(const File::Path& oldPath, const File::Path& newPath);

    private:
        inline static std::unordered_map<File::Path, FileContext*> _pathToGuidTable; // 파일 경로를 통해 ID를 찾는 테이블
        inline static std::unordered_map<File::Guid, FileContext*> _guidToPathTable; // ID를 통해 파일 경로를 찾는 테이블

        inline static std::unordered_set<FString> _ignoreExtTable; // 무시할 확장자 테이블
    };
} // namespace File