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
    enum Flags
    {
        ShowMetaFile,
        FlagSize,
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
    void ShowBrowserMenu();

    void ShowUpperFrame();

    void BeginColumn();
    void EndColumn();
    void ShowColumnPlitter();

    void ShowFolderHierarchy();
    void ShowFolderHierarchy(spForderContext forderContext);

    void ShowFolderContents();

    void ShowContentsToList();
    void ShowContentsToIcon();

    void ShowItemToList(spContext context);
    void ShowItemToIcon(spContext context);

    void ItemClickedAction(spContext context);

    

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
    std::array<bool, FlagSize> mAssetBrowserFlags = {false, };

    float _upperHeight = 30.0f;
    float _columWidth = 250.f;
    float _columHeight = 0.0f;
};

class EditorFileObject : public IEditorObject
{
public:
    virtual void OnInspectorStay() override;

public:
    inline auto GetContext() { return _context; }
    inline void SetContext(std::weak_ptr<File::Context> context) { _context = context; }

private:
    std::weak_ptr<File::Context> _context;
};