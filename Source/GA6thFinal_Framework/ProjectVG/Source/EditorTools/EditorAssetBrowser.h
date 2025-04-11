#pragma once
#include "UmFramework.h"

class EditorInspectorView;

namespace File
{
    class Context;
    class FileContext;
    class ForderContext;
}

class EditorFileObject;

class EditorAssetBrowser
    : public EditorTool
{
    enum eShowType
    {
        List,
        Icon,
    };
    enum eFlag
    {
        ShowMetaFile,
        FlagSize,
    };
    using wpForderContext = std::weak_ptr<File::ForderContext>;
    using spForderContext = std::shared_ptr<File::ForderContext>;
public:
    EditorAssetBrowser();
    virtual ~EditorAssetBrowser();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;
private:
    void ShowFolderHierarchy(const File::Path& folderPath);

    void ShowFolderContents();

    void ShowContentsToList();
    void ShowContentsToIcon();

    void BeginColum();
    void EndColum();
private:
    /* 브라우저에서 보여질 유형 (List, Icon) */
    eShowType mShowType;
    /* 현재 포커싱 폴더 */ 
    std::weak_ptr<File::ForderContext> _focusForder;
    /* 현재 선택된 폴더 or 파일 */
    std::shared_ptr<EditorFileObject> _selectedContext;

    /* 패널 위치 저장용 */
    float mPanelWidth = 200.0f;

    /* 각종 플래그 */
    std::array<bool, FlagSize> mAssetBrowserFlags = {false, };
};

class EditorFileObject : public IEditorObject
{
public:
    virtual void OnDrawInspectorView() override;

public:
    void SetContext(std::weak_ptr<File::Context> context);

private:
    std::weak_ptr<File::Context> _context;
};