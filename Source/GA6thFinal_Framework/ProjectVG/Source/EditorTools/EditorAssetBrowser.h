#pragma once
#include "UmFramework.h"

// 파일 및 폴더 구조 정의
struct FileNode {
    std::string name;
    bool isFolder;
    std::vector<FileNode> children;
};

class EditorAssetBrowser
    : public EditorTool
{
    using Path = std::filesystem::path;
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
public:
    EditorAssetBrowser();
    virtual ~EditorAssetBrowser();
private:
    virtual void OnStartGui() override;

    virtual void OnPreFrame() override;

    virtual void OnFrame() override;

    virtual void OnPostFrame() override;
private:
    void ShowFolderHierarchy(const Path& folderPath);

    void ShowFolderContents();

    void ShowContentsToList();
    void ShowContentsToIcon();

    void BeginColum();
    void EndColum();
private:
    /* 브라우저에서 보여질 유형 (List, Icon) */
    eShowType mShowType;
    /* 현재 포커싱 폴더 */ 
    Path mFocusFolderPath;
    /* 현재 선택된 폴더 or 파일 */
    Path mSelectedEntry;

    /* 패널 위치 저장용 */
    float mPanelWidth = 200.0f;

    /* 각종 플래그 */
    std::array<bool, FlagSize> mAssetBrowserFlags = {false, };
};