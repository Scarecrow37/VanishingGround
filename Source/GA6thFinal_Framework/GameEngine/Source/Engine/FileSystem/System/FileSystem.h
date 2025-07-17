#pragma once

namespace File
{
    class FileEventSubscriber;
    class FileEventObserver;
    class Context;
    class FileContext;
    class FolderContext;

    constexpr const char* UNDEFINED_EXTENSION       = ".UmUndefined";
    constexpr const char* PROJECT_EXTENSION         = ".UmProject";
    constexpr const char* PROJECT_SETTING_FILENAME  = "FileSystem.UmSetting";
} // namespace File

class EFileSystem
{
    using ContextSet            = std::unordered_set<std::shared_ptr<File::Context>>;
    using ContextPathTable      = std::unordered_map<File::Path, std::weak_ptr<File::Context>>;
    using ContextGuidTable      = std::unordered_map<File::Guid, std::weak_ptr<File::Context>>;
    using GuidRefTable          = std::unordered_map<File::Guid, std::size_t>;
    using EventSubscriberSet    = std::unordered_set<File::FileEventSubscriber*>;
    using EventSubscriberTable  = std::unordered_map<File::FString, EventSubscriberSet>;
    using CallBackFunc          = std::function<void(const File::FileEventData&)>;

public:
    EFileSystem();
    ~EFileSystem() = default;

public:
    bool LoadGameDirectory();
    bool CreateProject(const File::Path& path);
    bool LoadProject(const File::Path& path);
    bool SaveProject();
    bool SaveAsProject(const File::Path& to);
    int  LoadProjectWithMessageBox(const File::Path& path);
    int  SaveProjectWithMessageBox();

    bool SaveSetting(const File::Path& path);
    bool LoadSetting(const File::Path& path);

public:
    inline int                  GetDebugLevel()         const { return _setting.DebugLevel; }
    inline const std::string&   GetProjectName()        const { return _projectName; }
    inline const std::string&   GetMetaExt()            const { return _setting.MetaExt; }
    inline const File::Path&    GetOriginPath()         const { return _originPath; }
    inline const File::Path&    GetRootPath()           const { return _rootPath; }
    inline const File::Path&    GetAssetPath()          const { return _assetPath; }
    inline const File::Path&    GetProjectSettingPath() const { return _projectSettingPath; }
    inline const File::Path&    GetBuildSettingPath()   const { return _buildSettingPath; }

    bool IsLoadedProject() const;
    bool IsVaildGuid(const File::Guid& guid) const;
    bool IsValidExtension(const File::FString& ext) const;
    bool IsSameContext(std::weak_ptr<File::Context> left, std::weak_ptr<File::Context> right) const;
            
    File::Path                  GetRelativePath(const File::Path& path) const;
    const File::Path&           GetPathFromGuid(const File::Guid& guid) const;
    const File::Guid&           GetGuidFromPath(const File::Path& path) const;
    const GuidRefTable&         GetGuidRefTable() const;
    const EventSubscriberSet&   GetEventSubscribers(const File::FString& ext);

    template <typename T>
    std::weak_ptr<T> GetContext(const File::Guid& guid) const 
    {
        auto context = GetContext(guid);
        if (false == context.expired())
        {
            auto& path = GetPathFromGuid(guid);

            if constexpr (std::is_same_v<T, File::FolderContext>)
            {
                if (std::filesystem::is_directory(path))
                    return std::static_pointer_cast<T>(context.lock());
            }
            else if constexpr (std::is_same_v<T, File::FileContext>)
            {
                if (std::filesystem::is_regular_file(path))
                    return std::static_pointer_cast<T>(context.lock());
            }
        }
        return std::weak_ptr<T>();
    }
    std::weak_ptr<File::Context> GetContext(const File::Guid& guid) const;

    template <typename T>
    std::weak_ptr<T> GetContext(const File::Path& path) const
    {
        auto context = GetContext(path);
        if (false == context.expired())
        {
            if constexpr (std::is_same_v<T, File::FolderContext>)
            {
                if (std::filesystem::is_directory(path))
                    return std::static_pointer_cast<T>(context.lock());
            }
            else if constexpr (std::is_same_v<T, File::FileContext>)
            {
                if (std::filesystem::is_regular_file(path))
                    return std::static_pointer_cast<T>(context.lock());
            }
        }
        return std::weak_ptr<T>();
    }
    std::weak_ptr<File::Context> GetContext(const File::Path& path) const;
   
    void RequestInspectFile(const File::Path& path);
    void RequestOpenFile(const File::Path& path);
    void RequestCopyFile(const File::Path& path);
    void RequestPasteFile(const File::Path& path);
    void RequestDragDropFile(const File::Path& path);

    void DrawGuiSettingEditor();

public:
    void RegisterFileEventSubscriber(
        File::FileEventSubscriber* subscriber, const std::initializer_list<const char*>& exts = {});
    void UnRegisterFileEventSubscriber(File::FileEventSubscriber* subscriber);

public:
    void AddGuidRefCount(const File::Guid& guid);
    void SubGuidRefCount(const File::Guid& guid);
    std::size_t GetGuidRefCount(const File::Guid& guid) const;

public:
    void Clear();
    void ReadDirectory();
    void ReadDirectory(const File::Path& path);

    void ObserverSetUp(const CallBackFunc& callback);
    void ObserverShutDown();

    void RegisterContext(const File::Path& path);
    void UnregisterContext(const File::Path& path);
    void ProcessRemovedFile(const File::Path& path);
    void ProcessModifiedFile(const File::Path& path);
    void ProcessMovedFile(const File::Path& oldPath, const File::Path& newPath);

private:
    void ClearContext();
    void ClearEventSubscriber();
   
private:
    File::SystemSetting         _setting = {};              // 세팅 정보
    File::ProjectData           _projectData;               // 프로젝트 데이터
    std::string                 _projectName;               // 프로젝트 이름

    File::Path                  _originPath;                // 원본 경로(절대 경로)
    File::Path                  _rootPath;                  // 루트 경로(절대 경로)
    File::Path                  _assetPath;                 // 에셋 경로(절대 경로)
    File::Path                  _projectSettingPath;        // 프로젝트세팅 경로(절대 경로)
    File::Path                  _buildSettingPath;          // 빌드세팅 경로(절대 경로)

    File::FileEventObserver*    _observer = nullptr;        // 파일 디렉터리 이벤트를 감시하는 옵저버.

    ContextSet                  _contextTable;              // 원본 컨텍스트 포인터를 관리하는 테이블
    ContextPathTable            _pathToGuidTable;           // 파일 경로를 통해 ID를 찾는 테이블
    ContextGuidTable            _guidToPathTable;           // ID를 통해 파일 경로를 찾는 테이블
    GuidRefTable                _guidToRefTable;            // ID를 통해 참조를 찾는 테이블

    EventSubscriberSet          _subscriberSet;             // 등록된 EventSubscriber
    EventSubscriberTable        _extToSubscriberTable;      // 확장자를 통해 EventSubscriber를 찾는 테이블
};