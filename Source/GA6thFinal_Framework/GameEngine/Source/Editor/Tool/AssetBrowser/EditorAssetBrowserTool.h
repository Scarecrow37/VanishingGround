#pragma once

class EditorInspectorTool;

namespace File
{
    class Context;
    class FileContext;
    class ForderContext;
}

class EditorFileObject;

class EditorAssetBrowserTool
    : public EditorTool
{
    enum ShowType
    {
        List,
        Icon,
    };
    enum ShowFlags
    {
        ShowMetaFile,
        SHOW_FLAG_SIZE,
    };
    enum RenameFlags
    {
        RENAME_IS_RENAME = 0,   // 리네임 여부 플래그
        RENAME_SET_FOCUS_ONCE,  // 리네임 시 인풋 텍스트를 한번 포커싱해주기 위한 플래그
        RENAME_FALGS_SIZE,
    };
    using wpContext = std::weak_ptr<File::Context>;
    using spContext = std::shared_ptr<File::Context>;
    using wpFileContext = std::weak_ptr<File::FileContext>;
    using spFileContext = std::shared_ptr<File::FileContext>;
    using wpForderContext = std::weak_ptr<File::ForderContext>;
    using spForderContext = std::shared_ptr<File::ForderContext>;
public:
    EditorAssetBrowserTool();
    virtual ~EditorAssetBrowserTool();

private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;

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
    void ShowFolderHierarchy(spForderContext forderContext);

     /* 콘텐츠 뷰 콜럼 */
    void ShowFolderContents();

    void ShowContentsToList();
    void ShowContentsToIcon();

    void ShowItemToList(spContext context);
    void ShowItemToIcon(spContext context);

    void ItemInputText(spContext context);

    void ItemEventAction(spContext context);
    void ItemMouseAction(spContext context);
    void ItemKeyBoardAction(spContext context);
    void ItemPopupAction(spContext context);

    /* 팝업 박스 메서드 */
    void ShowDeletePopupBox(wpContext context);

private:
    /* 브라우저에서 보여질 유형 (List, Icon) */
    ShowType mShowType;
    /* 현재 포커싱 폴더 */ 
    std::weak_ptr<File::ForderContext> _focusForder;
    /* 현재 선택된 폴더 or 파일 */
    std::shared_ptr<EditorFileObject> _selectedContext;
    /* 패널 위치 저장용 */
    float mPanelWidth = 200.0f;
    /* 각종 플래그 */
    std::array<bool, SHOW_FLAG_SIZE> mAssetBrowserFlags = {false, };
    /* 이름 바꾸기 모드 여부 */
    std::bitset<RENAME_FALGS_SIZE> _renameFlags;

    float _upperHeight = 30.0f;
    float _columWidth = 250.f;
    float _columHeight = 0.0f;
};

class EditorFileObject : public IEditorObject
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