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
    constexpr const char* SETTING_EXTENSION         = ".UmSetting";
    constexpr const char* PROJECT_SETTING_FILENAME  = "FileSystem.UmSetting";
    constexpr const char* META_EXTENSION            = ".UmMeta";
} // namespace File

/*
EFileSystem 클래스는 프로젝트의 파일 시스템을 관리하는 핵심적인 역할을 수행하는 싱글톤(Singleton)
  객체입니다. 주요 기능은 다음과 같습니다.

    1. 프로젝트 관리:
        * 새로운 프로젝트를 생성(CreateProject)하거나 기존 프로젝트를 로드(LoadProject)하고 저장(SaveProject,
          SaveAsProject)하는 기능을 담당합니다.
        * 프로젝트의 이름, 루트 경로, 에셋 경로 등 주요 경로 정보를 관리합니다.

    2. 파일 및 디렉토리 관리:
        * 프로젝트 내의 파일과 디렉토리를 읽고(ReadDirectory), 각 파일/폴더에 대한 컨텍스트(Context) 정보를
          생성하고 관리합니다.
        * 파일 경로(Path), 전역 고유 식별자(Guid), 에셋 ID(AssetID) 간의 상호 변환 및 조회를 지원합니다. 이를
          통해 파일의 이동이나 이름 변경에 유연하게 대처할 수 있습니다.

    3. 파일 이벤트 처리:
        * FileEventObserver를 통해 파일 시스템의 변경(생성, 삭제, 수정, 이동)을 감시합니다.
        * 파일 이벤트가 발생하면, 해당 이벤트를 구독(RegisterFileEventSubscriber)하는 다른 시스템(Subscriber)에
           이를 알려 적절한 처리를 하도록 합니다. 예를 들어, 에디터에서 파일을 변경했을 때 관련 리소스를 다시
          로드하는 등의 작업을 수행할 수 있습니다.
        * 특정 파일 확장자(.UmProject, .UmMeta 등)에 대한 이벤트를 선별적으로 처리할 수 있습니다.

    4. 컨텍스트(Context) 관리:
        * 프로젝트 내의 모든 파일과 폴더에 대해 FileContext 또는 FolderContext라는 객체를 생성하여 관리합니다.
          이 컨텍스트는 파일의 메타데이터(MetaData), 경로, Guid 등의 정보를 포함합니다.
        * _pathToGuidTable, _guidToPathTable, _assetIDTable 등의 맵(Map) 자료구조를 사용하여 경로, Guid, 에셋
          ID를 빠르게 조회할 수 있도록 합니다.

    간단히 말해, `EFileSystem`은 에디터 또는 게임 엔진의 파일 탐색기와 같은 역할을 하는 객체입니다. 프로젝트의
    모든 파일과 폴더 구조를 파악하고, 파일의 생성/수정/삭제와 같은 변경 사항을 감지하여 다른 시스템에
    알려주는 중앙 허브 역할을 합니다. 이를 통해 프로젝트의 파일들을 체계적으로 관리하고, 파일 변경에 따른 후속
    작업을 자동화할 수 있습니다.
*/
class EFileSystem
{
    using ContextSet            = std::unordered_set<std::shared_ptr<File::Context>>;
    using ContextPathTable      = std::unordered_map<File::Path, std::weak_ptr<File::Context>>;
    using ContextGuidTable      = std::unordered_map<File::Guid, std::weak_ptr<File::Context>>;
    using AssetIDTable          = std::unordered_map<int, File::Path>;
    using EventSubscriberSet    = std::unordered_set<File::FileEventSubscriber*>;
    using EventSubscriberTable  = std::unordered_map<File::FString, EventSubscriberSet>;
    using CallBackFunc          = std::function<void(const File::FileEventData&)>;

public:
    EFileSystem();
    ~EFileSystem() = default;

public:
    /// <summary>
    /// <para>[PlayMode Only]</para>
    /// <para>플레이 모드에서 기본적으로 로드하는 작업을 수행합니다.</para>
    /// </summary>
    bool LoadGameDirectory();
    /// <summary>
    /// 지정된 경로에 새 프로젝트를 생성합니다. 기본적인 디렉토리 생성도 포함합니다.
    /// </summary>
    /// <param name="path">프로젝트를 생성할 파일 경로입니다.</param>
    /// <returns>프로젝트 생성에 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool CreateProject(const File::Path& path);
    /// <summary>
    /// 프로젝트를 지정된 경로에서 불러옵니다.
    /// </summary>
    /// <param name="path">불러올 프로젝트 파일의 경로입니다.</param>
    /// <returns>프로젝트를 성공적으로 불러오면 true, 실패하면 false를 반환합니다.</returns>
    bool LoadProject(const File::Path& path);
    /// <summary>
    /// 현재 프로젝트를 저장합니다.
    /// </summary>
    /// <returns>프로젝트 저장이 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool SaveProject();
    /// <summary>
    /// 다른 이름으로 현재 프로젝트를 저장합니다.
    /// </summary>
    /// <param name="to">새로 저장할 경로입니다.</param>
    /// <returns>프로젝트 저장이 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool SaveAsProject(const File::Path& to);

    int  LoadProjectWithMessageBox(const File::Path& path);
    int  SaveProjectWithMessageBox();

    bool SaveSetting(const File::Path& path);
    bool LoadSetting(const File::Path& path);

public:
    inline int                  GetDebugLevel()         const { return _setting.DebugLevel; }   // 파일 시스템의 디버그 레벨을 반환합니다.
    inline const std::string&   GetProjectName()        const { return _projectName; }          // 프로젝트 이름을 반환합니다.
    inline const File::Path&    GetOriginPath()         const { return _originPath; }           // 에디터가 실행된 경로를 반환합니다.
    inline const File::Path&    GetRootPath()           const { return _rootPath; }             // 프로젝트의 루트 경로를 반환합니다.
    inline const File::Path&    GetAssetPath()          const { return _assetPath; }            // 에셋 폴더의 경로를 반환합니다.
    inline const File::Path&    GetProjectSettingPath() const { return _projectSettingPath; }   // 프로젝트 설정 파일의 경로를 반환합니다.
    inline const File::Path&    GetBuildSettingPath()   const { return _buildSettingPath; }     // 빌드 설정 파일의 경로를 반환합니다.

    /// <summary>해당 경로를 상대 경로로 변환하여 반환합니다.</summary>
    File::Path                  GetRelativePath(const File::Path& path) const;
    /// <summary>Guid에 대응하는 Path를 받아옵니다. (File::Guid::ToPath를 사용하세요.)</summary>
    const File::Path&           GetPathFromGuid(const File::Guid& guid) const;
    /// <summary>Path에 대응하는 Guid를 받아옵니다. (File::Path::ToGuid를 사용하세요.)</summary>
    const File::Guid&           GetGuidFromPath(const File::Path& path) const;
    /// <summary>AssetID에 대응하는 Path를 받아옵니다. 유효하지 않는 AssetID면 빈 Path를 반환합니다.</summary>
    const File::Path&           GetPathFromAssetID(int assetID) const;
    /// <summary>AssetID에 대응하는 Guid를 받아옵니다. 유효하지 않는 AssetID면 빈 Guid를 반환합니다.</summary>
    const File::Guid&           GetGuidFromAssetID(int assetID) const;
    /// <summary>해당 확장자에 대응되는 EventSubscriber를 std::unordered_set으로 받아옵니다.</summary>
    const EventSubscriberSet&   GetEventSubscribers(const File::FString& ext);
    /// <summary>해당 경로에 대응되는 AssetID를 반환합니다. 실패할경우 0을 반환합니다.</summary>
    int GetAssetIDFromPath(const File::Path& path) const;
    /// <summary>해당 Guid에 대응되는 AssetID를 반환합니다. 실패할경우 0을 반환합니다.</summary>
    int GetAssetIDFromGuid(const File::Guid& guid) const;

    /// <summary>프로젝트가 로드되어있는지 확인합니다.</summary>
    bool IsLoadedProject() const;
    /// <summary>현재 프로젝트 기준에서 유효한 확장자인지 확인합니다. 확장자를 등록하려면 RegisterFileEventSubscriber를 통해 등록하세요.</summary>
    bool IsValidExtension(const File::FString& ext) const;
    /// <summary>같은 컨텍스트인지 비교합니다.</summary>
    bool IsSameContext(std::weak_ptr<File::Context> left, std::weak_ptr<File::Context> right) const;
    /// <summary>해당 에셋 ID가 이미 존재하는지 확인합니다.</summary>
    bool IsExistsAssetID(int assetID) const;
    /// <summary>해당 Guid가 이미 존재하는지 확인합니다.</summary>
    bool IsExistsGuid(const File::Guid& guid) const;
    /// <summary>해당 경로에 대응하는 컨텍스트가 있는지 확인합니다.</summary>
    bool HasContext(const File::Path& path) const;
    /// <summary>해당 Guid에 대응하는 컨텍스트가 있는지 확인합니다.</summary>
    bool HasContext(const File::Guid& guid) const;

    /// <summary>
    /// 파일 무결성을 검사합니다. Meta파일이 존재하는지 등.
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    void CheckFileContextIntegrity(const File::Path& path);

    /// <summary>
    /// 지정된 파일 경로의 에셋 ID를 변경합니다.
    /// </summary>
    /// <param name="path">에셋 ID를 변경할 파일의 경로입니다.</param>
    /// <param name="changeID">새로 설정할 에셋 ID입니다.</param>
    /// <returns>에셋 ID 변경이 성공하면 true, 실패하면 false를 반환합니다.</returns>
    bool ChangeAssetID(const File::Path& path, int changeID);
    bool ChangeAssetID(std::weak_ptr<File::Context> context, int changeID);

    /// <summary>
    /// 지정된 GUID에 해당하는 컨텍스트를 타입에 맞게 반환합니다.
    /// </summary>
    /// <typeparam name="T">반환할 컨텍스트의 타입입니다. 예: File::FolderContext 또는 File::FileContext.</typeparam>
    /// <param name="guid">컨텍스트를 조회할 파일의 GUID입니다.</param>
    /// <returns>요청한 타입과 일치하고 유효한 경우 해당 컨텍스트의 std::weak_ptr<T>를 반환합니다. 조건에 맞는 컨텍스트가 없거나 만료된 경우 빈 std::weak_ptr<T>를 반환합니다.</returns>
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
            else 
            {
                return context.lock();
            }
        }
        return std::weak_ptr<T>();
    }
    /// <summary>
    /// 지정된 guid에 해당하는 File::Context에 대한 weak_ptr를 반환합니다. 특정 타입의 Context를 반환해야 한다면 템플릿 함수를 사용하세요.
    /// </summary>
    /// <param name="guid">File::Context를 식별하는 File::Guid 참조입니다.</param>
    /// <returns>해당 guid에 연결된 File::Context 객체에 대한 std::weak_ptr입니다. 해당 컨텍스트가 존재하지 않으면 비어 있는 weak_ptr을 반환합니다.</returns>
    std::weak_ptr<File::Context> GetContext(const File::Guid& guid) const;

    /// <summary>
    /// 지정된 경로에 대한 컨텍스트를 가져옵니다.
    /// </summary>
    /// <typeparam name="T">반환할 컨텍스트의 타입입니다. 예: File::FolderContext 또는 File::FileContext.</typeparam>
    /// <param name="path">컨텍스트를 가져올 파일 또는 디렉터리의 경로입니다.</param>
    /// <returns>요청한 타입 T에 해당하는 컨텍스트의 std::weak_ptr입니다. 해당 컨텍스트가 없거나 타입이 일치하지 않으면 비어 있는 weak_ptr을 반환합니다.</returns>
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
            else 
            {
                return context.lock();
            }
        }
        return std::weak_ptr<T>();
    }
    /// <summary>
    /// 지정된 경로에 대한 File::Context에 대한 weak_ptr를 반환합니다. 특정 타입의 Context를 반환해야 한다면 템플릿 함수를 사용하세요.
    /// </summary>
    /// <param name="path">File::Context를 가져올 파일의 경로입니다.</param>
    /// <returns>해당 경로에 연결된 File::Context 객체의 std::weak_ptr입니다. 해당 컨텍스트가 존재하지 않으면 비어 있는 weak_ptr을 반환합니다.</returns>
    std::weak_ptr<File::Context> GetContext(const File::Path& path) const;
    std::weak_ptr<File::Context> GetContext(const std::filesystem::path& path) const;
    
    /// <summary>
    /// GUI 설정 편집기를 그립니다.
    /// </summary>
    void DrawGuiSettingEditor();

    ////////////////////////////////////////////////////
    /// 파일 이벤트 리퀘스트
    ////////////////////////////////////////////////////
    void RequestInspectFile(const File::Path& path);
    void RequestOpenFile(const File::Path& path);
    void RequestCopyFile(const File::Path& path);
    void RequestPasteFile(const File::Path& path);
    void RequestDragDropFile(const File::Path& path);


public:
    /// <summary>
    /// 파일 이벤트 구독자를 등록합니다. 확장자를 지정하지 않으면 모든 파일에 대해 이벤트를 수신합니다.
    /// </summary>
    /// <param name="subscriber">파일 이벤트를 받을 FileEventSubscriber상속 클래스 포인터입니다.</param>
    /// <param name="exts">파일 이벤트를 받을 확장자에 대한 배열입니다.</param>
    void RegisterFileEventSubscriber(File::FileEventSubscriber* subscriber, const std::initializer_list<const char*>& exts = {});
    /// <summary>
    /// 파일 이벤트 구독자를 등록 해제합니다.
    /// </summary>
    /// <param name="subscriber">등록 해제할 File::FileEventSubscriber 객체에 대한 포인터입니다.</param>
    void UnRegisterFileEventSubscriber(File::FileEventSubscriber* subscriber);

public:
    /// <summary>전체적인 정리를 실시합니다.</summary>
    void Clear();

    void ClearContext();
    void ClearEventSubscriber();
    
    /// <summary>루트 디렉토리부터 읽어와 컨텍스트를 등록합니다.</summary>
    void ReadDirectory();
    /// <summary>해당 디렉토리부터 하위 디렉토리까지 읽어 컨텍스트를 등록합니다.</summary>
    void ReadDirectory(const File::Path& path);

    /// <summary>파일 이벤트 옵저버를 시작합니다.</summary>
    void ObserverSetUp(const CallBackFunc& callback);
    /// <summary>파일 이벤트 옵저버를 종료합니다.</summary>
    void ObserverShutDown();

    ////////////////////////////////////////////////////
    /// FileContext 관련 처리
    ////////////////////////////////////////////////////

    void RegisterContext(const File::Path& path); // 컨텍스트를 등록합니다.
    void UnregisterContext(const File::Path& path); // 컨텍스트를 등록 해제합니다.
    void ProcessRemovedFile(const File::Path& path); // 파일이 삭제되었을 때 처리합니다.
    void ProcessModifiedFile(const File::Path& path); // 파일이 수정되었을 때 처리합니다.
    void ProcessMovedFile(const File::Path& oldPath, const File::Path& newPath); // 파일이 이동되었을 때 처리합니다.

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
    AssetIDTable                _assetIDTable;              // 에셋 ID를 통해 컨텍스트를 찾는 테이블

    EventSubscriberSet          _subscriberSet;             // 등록된 EventSubscriber
    EventSubscriberTable        _extToSubscriberTable;      // 확장자를 통해 EventSubscriber를 찾는 테이블
};