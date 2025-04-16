#pragma once

namespace File
{
    class FileEventNotifier;
    class FileObserver;
    class Context;
    class FileContext;
    class ForderContext;

    class FileSystem
    {
         using NotifierSet = std::unordered_set<File::FileEventNotifier*>;

    public:
        void SetDebugLevel(int level);
        int  GetDebugLevel();

        bool IsVaildGuid(const File::Guid& guid);
        bool IsValidExtension(const File::FString& ext);

        const File::Path& GetPathFromGuid(const File::Guid& guid);
        const File::Guid& GetGuidFromPath(const File::Path& path);

        template <typename T>
        std::weak_ptr<T> GetContext(const File::Guid& guid)
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
        std::weak_ptr<Context> GetContext(const File::Guid& guid);

        template <typename T>
        std::weak_ptr<T> GetContext(const File::Path& path)
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
        std::weak_ptr<Context> GetContext(const File::Path& path);

        NotifierSet GetNotifiers(const File::FString& ext);

    public:
        void RegisterFileEventNotifier(FileEventNotifier* notifier,
            const std::initializer_list<std::string>& exts);
        void UnRegisterFileEventNotifier(FileEventNotifier* notifier);

    public:
        void Clear();
        void ReadDirectory(const File::Path& path);
        void AddedFile(const File::Path& path);
        void RemovedFile(const File::Path& path);
        void ModifiedFile(const File::Path& path);
        void MovedFile(const File::Path& oldPath, const File::Path& newPath);

    private:
        std::unordered_set<std::shared_ptr<Context>> _contextTable;                 // 원본 컨텍스트 포인터를 관리하는 테이블
        std::unordered_map<File::Path, std::weak_ptr<Context>> _pathToGuidTable;    // 파일 경로를 통해 ID를 찾는 테이블
        std::unordered_map<File::Guid, std::weak_ptr<Context>> _guidToPathTable;    // ID를 통해 파일 경로를 찾는 테이블
        std::unordered_map<FString, NotifierSet> _notifierTable;                    // Notifier 테이블

        int _debugLevel = 0;
    };
} // namespace File