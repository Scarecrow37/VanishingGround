#pragma once

class EditorInspectorTool;

namespace File
{
    class Context;
    class FileContext;
    class FolderContext;
}

class EditorAssetObject;

class EditorAssetBrowserTool
    : public EditorTool
    , public File::FileEventSubscriber
{
    using FileEntry = std::filesystem::directory_entry;
    using FileTime  = std::filesystem::file_time_type;

    enum ShowType
    {
        SHOW_TYPE_LIST,
        SHOW_TYPE_ICON,
        SHOW_TYPE_EDIT,
    };
    enum Flags
    {
        FLAG_SHOW_META          = 1,     // 메타 파일 표시 여부
        FLAG_SIZE,
    };
    struct AssetData
    {
        AssetData(FileEntry entry);
        AssetData()  = default;
        ~AssetData() = default;

        void Refresh(FileEntry entry);
        bool IsSamePath(const std::filesystem::path& path) const;

        FileEntry   Entry;                      // 파일 엔트리
        bool        IsDirectory = false;        // 디렉토리인지 여부
        std::string Extension = "";             // 파일 확장자
        std::string FileName = "";              // 파일 이름
        std::string ViewName = "";              // 뷰에 표시될 이름
        std::time_t LastWriteTime = {};         // 마지막 수정 시간
        int         Order = 0;                  // 정렬 순서
        bool        AbleToDragSource = true;    // 드래그 소스 가능 여부
        bool        AbleToDragTarget = true;    // 드래그 타겟 가능 여부

        // icon
        std::shared_ptr<Texture> PreviewIconTexture; // 아이콘 텍스처
    };
    using wpContext = std::weak_ptr<File::Context>;
    using spContext = std::shared_ptr<File::Context>;
    using wpFileContext = std::weak_ptr<File::FileContext>;
    using spFileContext = std::shared_ptr<File::FileContext>;
    using wpFolderContext = std::weak_ptr<File::FolderContext>;
    using spFolderContext = std::shared_ptr<File::FolderContext>;
public:
    EditorAssetBrowserTool();
    virtual ~EditorAssetBrowserTool();

    static EditorAssetBrowserTool* GetInstance();

private:
    inline static EditorAssetBrowserTool* _staticInstance = nullptr;
    static constexpr ImVec2               ICON_WIDGET_SIZE = ImVec2(100.0f, 100.0f); // 아이콘 위젯 크기
    static constexpr const char*          POPUP_ID         = "##popup";

public:
    /* Static 메서드 */
    static bool ChangeAssetID(const File::Path& path, int changeID);
    static bool ChangeAutomaticAssetID(const AssetData& asset);

public:
    inline const File::Path& GetCurrentFocusFolderPath() const { return _focusFolderPath; }
    AssetData* GetAssetData(const File::Path& path);

private:
    void OnTickGui() override;
    void OnStartGui() override;

    void OnPreFrameBegin() override;
    void OnPostFrameBegin() override;
    void OnFrameRender() override;
    void OnFrameEnd() override;
    void OnFrameFocusEnter() override;
    void OnFrameFocusStay() override;
    void OnFrameFocusExit() override;

    void OnPostRequestedLoad() override;

public:
    void ResetState();
    void ProcessInput();
    void RefreshFocusFolderEntries();
    void SetFocusFolderPath(const File::Path& path, bool pushStack = true);
    void SetFocusEntryPath(const File::Path& path);
    void ClearUndoRedoStack();
    void UndoPath();
    void RedoPath();
    void SetCopyFileFromPath(const File::Path& path);
    void SetCutFileFromPath(const File::Path& path);
    void PasteFile();
    bool DeleteFileFromPath(const File::Path& path);
         
    bool IsFavoriteFolder(const File::Path& path) const;
    void AddFavoriteFolder(const File::Path& path);
    void RemoveFavoriteFolder(const File::Path& path);

private:
    /* 메뉴바 - 콜럼 사이 어퍼프레임 */
    void ShowUpperFrame();

    /* 좌측, 우측 컬럼 */
    void BeginColumn();         // Begin
    void EndColumn();           // End
    void ShowColumnPlitter();   // 콜럼 사이 리사이징바

    /* 폴더 계층 뷰 콜럼 */
    void ShowFolderHierarchy();
    void ShowFolderHierarchyTree(const File::Path& directory);

    /* 콘텐츠 뷰 콜럼 */
    void ShowFolderEntries();
    void UpdateFolderEntriesInput();
    void ShowSearchBar();   
    void ShowFolderEntryToList(AssetData& asset);  
    void ShowFolderEntryToIcon(AssetData& asset);  
    void ShowFolderEntryToEdit(AssetData& asset);  
    void ShowFolderEntryPopup(AssetData& asset);
    void ProcessFolderEntryDragDrop(AssetData& asset);
    void BeginFolderEntryFrame();

    /* 팝업 박스 메서드 */
    void ShowSameFilePopupBox();
    void ShowCopyFilePopupBox();
    static bool ShowDeletePopupBox(const File::Path& path);
    static void ShowAlreadyAssetIDPopupBox(const File::Path& path, int changeID);

private:
    void ProcessDropFile(const HDROP hDrop);
    static bool WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

 
private:
    int         _folderCount    = 0;                    // 폴더 개수
    float       _zoomScale      = 1.0f;                 // 콘텐츠 뷰 줌 스케일
    float       _updateTime     = 0.0f;                 // 콘텐츠 뷰 업데이트 시간
    bool        _needRefresh    = false;                // 콘텐츠 뷰 새로고침 필요 여부
    File::Path  _focusFolderPath;                       // 현재 포커싱 중인 폴더
    File::Path  _focusEntryPath;                        // 현재 포커싱 중인 파일
    
    std::bitset<FLAG_SIZE> _flags;                      // 플래그 비트셋 (예: 메타 파일 표시 여부 등)

    std::pair<int, File::Path> _copyBuffer = {-1, ""};   // 복사 버퍼 (first가 -1이면 비어있음/동작 없음, 0이면 복사, 1이면 잘라넣기)
    std::vector<std::function<void()>> _delayEvent;     // 후처리 이벤트 (보통 삭제나 추가 등의 작업을 함)

    /* 에셋 정보 저장 테이블 및 리스트 */
    std::unordered_map<File::Path, AssetData> _focusFolderAssetDataMap; // 현재 포커싱 폴더의 파일 목록 맵 (경로 -> AssetData)
    std::vector<AssetData*> _focusFolderAssetDataList;  // 현재 포커싱 폴더의 파일 목록
    
    /* Undo 및 Redo 관련 */
    std::deque<File::Path> _directoryUndoStack;
    std::deque<File::Path> _directoryRedoStack;
    int                    _maxUndoStack = 20; // Undo Stack 최대 개수

    /* 외부 파일 Drag & Drop 이벤트 관련 */
    std::vector<std::pair<bool, File::Path>> _dragDropPaths; // 드래그 앤 드롭된 파일 경로들 (복사 여부, 경로)
    File::Path _dragDropPath; // 드래그 앤 드롭된 경로의 목적지 경로


    struct SearchController
    {
        bool IsActive() const { return IsSearching; } // 검색 모드인지 확인
        void UpdateBuffer();
        void ClearBuffer();
        bool PassFilter(const char* text);

        bool IsSearching = false;       // 검색 모드 여부
        std::string SearchBuffer = "";  // 검색 버퍼
        ImGuiTextFilter SearchFilter;   // ImGui 검색 필터
    };
    SearchController _search; // 검색 컨트롤러

    /// <summary>
    /// 이름 변경을 할 때 사용함.
    /// </summary>
    struct RenameController
    {
        bool              IsActive() const { return IsRenaming; }      // 이름 변경 모드인지 확인
        void              StartRename(const File::Path& destFilePath); // 이름 변경 시작
        void              CancelRename();                              // 이름 변경 취소
        const File::Path& ExecuteRename(const File::Path& destFolder);

        bool        IsRenaming   = false; // 이름 변경 모드 여부
        File::Path  Return       = "";    // 이름 변경 대상 이름
        File::Path  DestPath     = "";    // 이름 변경 대상 이름
        std::string DestName     = "";    // 이름 변경 대상 이름
        std::string RenameBuffer = "";    // 이름 변경 버퍼
    };
    RenameController _rename; // 이름 변경 컨트롤러

    /// <summary>
    /// AssetData를 받아 정렬하는 비교 함수입니다.
    /// </summary>
    class Compare
    {
    public:
        enum SortFlags
        {
            FLAGS_SORT_BY_NONE      = 0,      // 정렬 없음
            FLAGS_SORT_BY_TYPE      = 1 << 1, // 유형별 정렬
            FLAGS_SORT_BY_NAME      = 1 << 2, // 이름순 정렬
            FLAGS_SORT_BY_DATE      = 1 << 3, // 날짜순 정렬
            FLAGS_SORT_BY_ASSET_ID  = 1 << 3, // 에셋 ID순 정렬
        };

    public:
        Compare(int sortFlags = 0, bool ascending = true) : flags(sortFlags), isAscending(ascending) {}
        Compare(std::pair<int, bool>& setting) : flags(setting.first), isAscending(setting.second) {}
        ~Compare() = default;
        bool operator()(const AssetData* a, const AssetData* b) const;

    private:
        bool CompareByType(const AssetData* a, const AssetData* b) const;
        bool CompareByName(const AssetData* a, const AssetData* b) const;
        bool CompareByDate(const AssetData* a, const AssetData* b) const;
        bool CompareByAssetID(const AssetData* a, const AssetData* b) const;

    private:
        int flags = 0; // 정렬 플래그 (예: 이름순, 날짜순 등)
        bool isAscending = true;
    };

    /// <summary>
    /// 인스펙터에 표시할 에셋 정보를 드로어하는 클래스입니다.
    /// </summary>
    class InspectorDrawer : public IEditorObject
    {
    public:
        void OnInspectorEnter() override;
        void OnInspectorStay() override;
        void OnInspectorExit() override;

        void SetAsset(AssetData* assetData);
    public:
        AssetData* _assetData;
        File::Path _assetPath;
        std::weak_ptr<File::Context> _selectedAsset;

    };
    std::shared_ptr<InspectorDrawer> _inspectorDrawer; // 인스펙터 정보 드로어

    // ReflectFields
    REFLECT_FIELDS_BEGIN(EditorTool)
    float                 ColumWidth  = 250.f;
    float                 ColumHeight = 0.0f;       
    int                   ShowType    = SHOW_TYPE_LIST;                         // 현재 보여지는 타입 (리스트, 아이콘 등)
    std::pair<int, bool>  SortFlags   = {Compare::FLAGS_SORT_BY_NAME, true};    // 정렬 플래그 (예: 이름순, 날짜순 등)
    std::set<std::string> FavoriteFolders;                                      // 즐겨찾기 폴더 목록
    std::array<float, 3>  ListColumnWidth = {0.4f, 0.65f, 0.75f};               // 리스트 컬럼 비율 (이름, 마지막 수정 날짜, 에셋 ID)
    REFLECT_FIELDS_END(EditorAssetBrowserTool)
};