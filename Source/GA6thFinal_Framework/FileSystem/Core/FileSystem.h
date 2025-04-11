#pragma once

namespace File
{
    class FileObserver;
    class Context;
    class FileContext;
    class ForderContext;

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

        /* Root 패스 반환 */
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
        static bool              IsVaildGuid(const File::Guid& guid);
        static bool              IsVaildExtension(const File::Path& path);

        static const File::Path& GetPathFromGuid(const File::Guid& guid);
        static const File::Guid& GetGuidFromPath(const File::Path& path);

        static std::weak_ptr<Context> GetContext(const File::Guid& guid);
        static std::weak_ptr<Context> GetContext(const File::Path& path);

        template <typename T>
        static std::weak_ptr<T> GetContext(const File::Guid& guid)
        {
            auto context = GetContext(guid);
            if (false == context.expired())
            {
                auto& path   = GetPathFromGuid(guid);

                if constexpr (std::is_same_v<T, ForderContext>)
                {
                    if (stdfs::is_directory(path))
                        return std::static_pointer_cast<T>(context.lock());
                }
                else if constexpr (std::is_same_v<T, FileContext>)
                {
                    if (stdfs::is_regular_file(path))
                        return std::static_pointer_cast<T>(context.lock());
                }
            }
            return std::weak_ptr<T>();
        }
        template <typename T>
        static std::weak_ptr<T> GetContext(const File::Path& path)
        {
            auto context = GetContext(path);
            if (false == context.expired())
            {
                if constexpr (std::is_same_v<T, ForderContext>)
                {
                    if (stdfs::is_directory(path))
                        return std::static_pointer_cast<T>(context.lock());
                }
                else if constexpr (std::is_same_v<T, FileContext>)
                {
                    if (stdfs::is_regular_file(path))
                        return std::static_pointer_cast<T>(context.lock());
                }
            }
            return std::weak_ptr<T>();
        }

    public:
        static void Clear();
        static void AddedFile(const File::Path& path);
        static void RemovedFile(const File::Path& path);
        static void ModifiedFile(const File::Path& path);
        static void MovedFile(const File::Path& oldPath, const File::Path& newPath);

    private:
        inline static std::unordered_map<File::Path, std::weak_ptr<Context>> _pathToGuidTable; // 파일 경로를 통해 ID를 찾는 테이블
        inline static std::unordered_map<File::Guid, std::weak_ptr<Context>> _guidToPathTable; // ID를 통해 파일 경로를 찾는 테이블

        inline static std::unordered_set<std::shared_ptr<Context>> _contextTable; // 원본 포인터를 관리하는 테이블

        inline static std::unordered_set<FString> _ignoreExtTable; // 무시할 확장자 테이블
    };
} // namespace File