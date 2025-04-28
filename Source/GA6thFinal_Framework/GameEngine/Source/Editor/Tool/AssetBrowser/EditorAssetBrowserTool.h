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
{
    enum ShowType
    {
        List,
        Icon,
    };
    enum Flags
    {
        FLAG_IS_RENAME = 0,     // 리네임 중인지 여부

        FALG_SIZE,
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

private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

    virtual void OnFocus() override;

private:
    /* 브라우저 메뉴바 */
    void ShowBrowserMenu();

    /* 메뉴바 - 콜럼 사이 어퍼프레임 */
    void ShowUpperFrame();

    /*  */
    void BeginColumn();         // Begin
    void EndColumn();           // End
    void ShowColumnPlitter();   // 콜럼 사이 리사이징바

    /* 폴더 계층 뷰 콜럼 */
    void ShowFolderHierarchy();
    void ShowFolderHierarchy(spFolderContext FolderContext);

     /* 콘텐츠 뷰 콜럼 */
    void ShowFolderContents();

    void ShowFolderDirectoryPath(spFolderContext context);  // 콘텐츠 뷰 상단 주소 출력
    void ContentsFrameEventAction(spFolderContext context); // 콘텐츠 뷰 프레임 이벤트 액션

    void ShowContentsToList(); // 콘텐츠 뷰 출력 타입 - 리스트
    void ShowContentsToIcon(); // 콘텐츠 뷰 출력 타입 - 아이콘

    void ShowItemToList(spContext context, const char* mode = ""); // 콘텐츠 뷰 아이템 출력 - 리스트 
    void ShowItemToIcon(spContext context, const char* mode = ""); // 콘텐츠 뷰 아이템 출력 - 아이콘 

    void ItemInputText(spContext context);  // 콘텐츠 뷰 이름 변경 인풋 텍스트

    void ItemEventAction(spContext context, const char* mode = "");    // 콘텐츠 뷰 아이템 이벤트 액션
    void ItemInputAction(spContext context, const char* mode = "");    // 콘텐츠 뷰 아이템 인풋 액션
    void ItemPopupAction(spContext context, const char* mode = "");    // 콘텐츠 뷰 아이템 팝업 액션

    /* 팝업 박스 메서드 */
    void ShowDeletePopupBox(wpContext context);
    void ShowSameFilePopupBox();


private:
    void ProcessEnterAction(spContext context);
    void ProcessMoveAction(wpContext srcContext, wpFolderContext dstContext);

    void SetFocusInspector(wpContext context);
    bool SetFocusFolder(wpFolderContext context); // 선택된 폴더 or 파일 포커싱
    void SetFocusParentFolder(spContext context);
    void SetFocusFromUndoPath();
    void SetFocusFromRedoPath();

private:
    bool IsKeyDownCopy();
    bool IsKeyDownPaste();

private:
    /* 브라우저에서 보여질 유형 (List, Icon) */
    ShowType _showType;
    /* 현재 포커싱 폴더 */
    File::Path      _currFocusFolderPath;
    wpFolderContext _currFocusFolderContext;
    wpFolderContext _nextFocusFolderContext;
    /* 현재 선택된 폴더 or 파일 */
    std::shared_ptr<EditorAssetObject> _selectedContext;
    /* 이름 바꾸기 모드 여부 */
    std::bitset<FALG_SIZE> browserFlags;

    /* Undo, Redo 스택 */ 
    int                    _maxUndoStack = 20; // Undo Stack 최대 개수
    std::deque<File::Path> _directoryUndoStack;
    std::deque<File::Path> _directoryRedoStack;

    /* Copy&Paste */
    File::Path _copyPath;
    
    /* EventProcessing */
    std::vector<std::function<void()>> _eventFunc; 

    // ReflectFields
    REFLECT_FIELDS_BEGIN(EditorTool)
    float UpperHeight = 40.0f;
    float ColumWidth  = 250.f;
    float ColumHeight = 0.0f;
    REFLECT_FIELDS_END(EditorAssetBrowserTool)
};

class EditorAssetObject : public IEditorObject
{
public:
    virtual void OnInspectorStay() override;

public:
    inline auto GetContext() 
    {
        return _context; 
    }
    inline void SetContext(std::weak_ptr<File::Context> context) 
    {
        _context = context; 
    }

private:
    std::weak_ptr<File::Context> _context;
};