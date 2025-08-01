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

        void Refesh(FileEntry entry);

        FileEntry   Entry;              // 파일 엔트리
        bool        IsDirectory;        // 디렉토리인지 여부
        std::string FileName;           // 파일 이름
        std::string ViewName;           // 뷰에 표시될 이름
        std::time_t LastWriteTime;      // 마지막 수정 시간
        int         Order = 0;          // 정렬 순서 

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

public:
    inline const File::Path& GetCurrentFocusFolderPath() const { return _focusFolderPath; }
    inline const ImRect&     GetWindowRect() const { return _windowRect; }

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

private:
    /* 메뉴바 - 콜럼 사이 어퍼프레임 */
    void ShowUpperFrame();
    void ShowFolderDirectoryPath(spFolderContext context); // 주소 출력
    void ListToDirectoryFileName(const File::Path& relativePath);

    /*  */
    void BeginColumn();         // Begin
    void EndColumn();           // End
    void ShowColumnPlitter();   // 콜럼 사이 리사이징바

    /* 폴더 계층 뷰 콜럼 */
    void ShowFolderHierarchy();
    void ShowFolderHierarchyTree(const File::Path& directory);

    /* 콘텐츠 뷰 콜럼 */
    void ShowFolderEntries();
    void ShowFolderEntryToList(AssetData& asset);  
    void ShowFolderEntryToIcon(AssetData& asset);  
    void ShowFolderEntryPopup(AssetData& asset);
    void ProcessFolderEntryDragDrop(AssetData& asset);
    void UpdateFolderEntryInput();

    void ShowSearchBar(spFolderContext context);   

    /* 팝업 박스 메서드 */
    void ShowDeletePopupBox(const File::Path& path);
    void ShowSameFilePopupBox();
    void ShowCopyFilePopupBox();

private:
    void SetFocusFromUndoPath();
    void SetFocusFromRedoPath();

    void ProcessDropFile(const HDROP hDrop);
    static bool WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    AssetData* GetAssetData(const File::Path& path);
    void RefreshState();
    void RefreshFocusFolderEntries();
    void SetFocusFolderPath(const File::Path& path);
    void SetFocusEntryPath(const File::Path& path);
 
    // 최신 개발
    int         _folderCount    = 0;        // 폴더 개수
    float       _zoomScale      = 1.0f;     // 콘텐츠 뷰 줌 스케일
    float       _updateTime     = 0.0f;     // 콘텐츠 뷰 업데이트 시간
    bool        _needRefresh    = false;    // 콘텐츠 뷰 새로고침 필요 여부
    File::Path  _copyBuffer     = "";       // 복사 버퍼
    File::Path  _focusFolderPath;           // 현재 포커싱 중인 폴더
    File::Path  _focusEntryPath;            // 현재 포커싱 중인 파일
    
    std::bitset<FLAG_SIZE> _flags;          // 플래그 비트셋 (예: 메타 파일 표시 여부 등)

    std::unordered_map<File::Path, AssetData> _focusFolderAssetDataMap;
    std::vector<AssetData*> _focusFolderAssetDataList;  // 현재 포커싱 폴더의 파일 목록

    std::vector<std::function<void()>> _delayEvent;     // 후처리 이벤트 

    static constexpr ImVec2 ICON_WIDGET_SIZE = ImVec2(100.0f, 100.0f); // 아이콘 위젯 크기
    static constexpr const char* POPUP_ID    = "##popup";

    struct RenameController
    {
        bool IsActive() const { return IsRenaming; } // 이름 변경 모드인지 확인
        void StartRename(const File::Path& destFilePath); // 이름 변경 시작
        void CancelRename(); // 이름 변경 취소
        const File::Path& ExecuteRename(const File::Path& destFolder);
        
        bool        IsRenaming   = false;   // 이름 변경 모드 여부
        File::Path  Return       = "";      // 이름 변경 대상 이름
        File::Path  DestPath     = "";      // 이름 변경 대상 이름
        std::string DestName     = "";      // 이름 변경 대상 이름
        std::string RenameBuffer = "";      // 이름 변경 버퍼

    };
    RenameController _rename; // 이름 변경 컨트롤러

private:
    /* Undo, Redo 스택 */ 
    int                    _maxUndoStack = 20; // Undo Stack 최대 개수
    std::deque<File::Path> _directoryUndoStack;
    std::deque<File::Path> _directoryRedoStack;

    /* Search */
    char _searchBuffer[128] = "";

    /* Drag&Drop */
    ImRect _windowRect;
    std::vector<std::pair<bool, File::Path>> _dragDropPaths; // 드래그 앤 드롭된 파일 경로들 (복사 여부, 경로)
    File::Path _destPath; // 드래그 앤 드롭된 경로의 목적지 경로

    // ReflectFields
    REFLECT_FIELDS_BEGIN(EditorTool)
    float ColumWidth  = 250.f;
    float ColumHeight = 0.0f;
    int   SortFlags   = File::Compare::FLAGS_SORT_BY_TYPE | File::Compare::FLAGS_SORT_BY_NAME;
    int   ShowType    = SHOW_TYPE_LIST;
    REFLECT_FIELDS_END(EditorAssetBrowserTool)

    class Compare
    {
    public:
        enum SortFlags
        {
            FLAGS_SORT_BY_NONE = 0,      // 정렬 없음
            FLAGS_SORT_BY_TYPE = 1 << 1, // 유형별 정렬
            FLAGS_SORT_BY_NAME = 1 << 2, // 이름순 정렬
            FLAGS_SORT_BY_DATE = 1 << 3, // 날짜순 정렬
        };

    public:
        Compare(int sortFlags = 0) : flags(sortFlags) {}
        ~Compare() = default;
        bool operator()(const AssetData* a, const AssetData* b) const;

    private:
        bool CompareByType(const AssetData* a, const AssetData* b) const;
        bool CompareByName(const AssetData* a, const AssetData* b) const;
        bool CompareByDate(const AssetData* a, const AssetData* b) const;

    private:
        int flags = 0; // 정렬 플래그 (예: 이름순, 날짜순 등)
    };
};

class EditorAssetObject : public IEditorObject
{
public:
    virtual void OnInspectorEnter() override;
    virtual void OnInspectorStay() override;
    virtual void OnInspectorExit() override;

public:
    inline void SetThis(std::weak_ptr<EditorAssetObject> thisObj)
    { 
        _this = thisObj; 
    }

    inline auto GetContext() 
    {
        return _selectedAsset; 
    }

    void SetContext(std::weak_ptr<File::Context> context);

private:
    std::weak_ptr<File::Context> _selectedAsset;
    std::weak_ptr<File::Context> _focusedInspector;

    std::weak_ptr<EditorAssetObject> _this; // 자신 weak_ptr 객체
};

