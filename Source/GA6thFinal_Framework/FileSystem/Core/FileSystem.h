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
        static void SetDebugLevel(int level);
        static int  GetDebugLevel();

        static bool IsVaildGuid(const File::Guid& guid);
        static bool IsValidExtension(const File::FString& ext);

        static const File::Path& GetPathFromGuid(const File::Guid& guid);
        static const File::Guid& GetGuidFromPath(const File::Path& path);

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
        static std::weak_ptr<Context> GetContext(const File::Guid& guid);

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
        static std::weak_ptr<Context> GetContext(const File::Path& path);

        static NotifierSet GetNotifiers(const File::FString& ext);
    public:
        static void RegisterFileEventNotifier(FileEventNotifier* notifier);
        static void UnRegisterFileEventNotifier(FileEventNotifier* notifier);

    public:
        static void Clear();
        static void ReadDirectory(const File::Path& path);
        static void AddedFile(const File::Path& path);
        static void RemovedFile(const File::Path& path);
        static void ModifiedFile(const File::Path& path);
        static void MovedFile(const File::Path& oldPath, const File::Path& newPath);

    private:
        inline static std::unordered_set<std::shared_ptr<Context>> _contextTable; // 원본 컨텍스트 포인터를 관리하는 테이블
        
        inline static std::unordered_map<File::Path, std::weak_ptr<Context>> _pathToGuidTable; // 파일 경로를 통해 ID를 찾는 테이블
        inline static std::unordered_map<File::Guid, std::weak_ptr<Context>> _guidToPathTable; // ID를 통해 파일 경로를 찾는 테이블
    
        inline static std::unordered_map<FString, NotifierSet> _notifierTable;

        inline static int _debugLevel = 0;
    };
} // namespace File