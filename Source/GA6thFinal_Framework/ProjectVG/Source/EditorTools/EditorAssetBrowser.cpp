#include "EditorAssetBrowser.h"
#include "EditorInspectorView.h"
#include "../FileSystem/Extra/FileContext.h"

namespace fs = std::filesystem;

EditorAssetBrowser::EditorAssetBrowser()
{
    SetLabel("AssetBrowser");
    SetInitialDockSpaceArea(DockSpaceArea::DOWN);

    mShowType = List;
    _selectedContext = std::make_shared<EditorFileObject>();
    //mFocusFolderPath = Asset::System::GetRootAssetPath();
}

EditorAssetBrowser::~EditorAssetBrowser()
{
}

void EditorAssetBrowser::OnStartGui()
{
}

void EditorAssetBrowser::OnPreFrame()
{
}

void EditorAssetBrowser::OnFrame()
{
    BeginColum();

    // 왼쪽: 폴더 트리
    ImGui::BeginChild("FolderHierarchyView", ImVec2(0, 0), true);
    ShowFolderHierarchy(Global::fileSystem->GetRootPath());
    ImGui::EndChild();

    ImGui::NextColumn();

    // 오른쪽: 선택한 폴더의 파일 목록
    ImGui::BeginChild("ContentsView", ImVec2(0, 0), true);
    ShowFolderContents();
    ImGui::EndChild();

    EndColum();
}

void EditorAssetBrowser::OnPostFrame()
{
}

// 왼쪽: 폴더 트리 표시 (재귀)
void EditorAssetBrowser::ShowFolderHierarchy(const File::Path& folderPath)
{
    auto wpContext = File::IDMapper::GetContext<File::ForderContext>(folderPath);
    if (false == wpContext.expired())
    {
        auto spForderContext = wpContext.lock();
        for (auto itr = spForderContext->begin(); itr != spForderContext->end();
             ++itr)
        {
            auto& [name, wpFileContext] = *itr;
            if (false == wpFileContext.expired())
            {
                auto spFileContext = wpFileContext.lock();
                if (true == spFileContext->IsDirectory())
                {
                    std::string id =
                        spFileContext->GetPath().filename().string() + "##" +
                        spFileContext->GetPath().string();

                    bool isOpen = ImGui::TreeNodeEx(
                        id.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
                    if (ImGui::IsItemHovered() &&
                        ImGui::IsMouseDoubleClicked(0))
                    {
                        _focusForder =
                            File::IDMapper::GetContext<File::ForderContext>(
                                spFileContext->GetPath());
                    }
                    if (isOpen)
                    {
                        ShowFolderHierarchy(
                            spFileContext->GetPath()); // 재귀적으로 하위 폴더 표시
                        ImGui::TreePop();
                    }
                }
            }
        }
    }
}

// 오른쪽: 선택된 폴더의 파일 목록
void EditorAssetBrowser::ShowFolderContents()
{
    switch (mShowType)
    {
    case EditorAssetBrowser::List:
        ShowContentsToList();
        break;
    case EditorAssetBrowser::Icon:
        ShowContentsToIcon();
        break;
    default:
        break;
    }
}

void EditorAssetBrowser::ShowContentsToList()
{
    // 참조 포인터가 살아있을 때
    if (false == _focusForder.expired())
    {
        auto spFocusContext = _focusForder.lock();

        for (auto itr = spFocusContext->begin(); itr != spFocusContext->end(); ++itr)
        {
            auto& [name, wpFileContext] = *itr;
            if (false == wpFileContext.expired())
            {
                auto spFileContext = wpFileContext.lock();

                File::Path  path = spFileContext->GetPath();
                std::string id   = name.string() + "##" + path.string();

                bool isMeta = path.extension() == File::MetaData::EXTENSION;

                if (true == mAssetBrowserFlags[ShowMetaFile] ||
                    (false == mAssetBrowserFlags[ShowMetaFile] &&
                     false == isMeta))
                {
                    if (ImGui::Selectable(id.c_str()))
                    {
                        _selectedContext->SetContext(spFileContext);
                        EditorInspectorView::SetFocusObject(_selectedContext);
                    }
                    // 더블 클릭시 파일 열기
                    if (ImGui::IsItemHovered() &&
                        ImGui::IsMouseDoubleClicked(0))
                    {
                        ShellExecute(NULL, L"open", path.wstring().c_str(),
                                     NULL, NULL, SW_SHOW);
                    }
                }
            }
        }
    }
}

void EditorAssetBrowser::ShowContentsToIcon()
{
}


void EditorAssetBrowser::BeginColum()
{
    ImGui::Columns(2, "AssetBrowser", true);
    //ImGui::SetColumnWidth(0, mPanelWidth);
}

void EditorAssetBrowser::EndColum()
{
    // 크기 변경 감지 & 저장
    //if (mPanelWidth != ImGui::GetColumnWidth(0))
    //{
    //    mPanelWidth = ImGui::GetColumnWidth(0);
    //} 

    ImGui::Columns(1); // 컬럼 종료
}

void EditorFileObject::OnDrawInspectorView()
{
    if (false == _context.expired())
    {
        auto spContext = _context.lock();
        const File::MetaData& meta = spContext->GetMeta();

        ImGui::Text("Path: %s", spContext->GetPath().string().c_str());
        ImGui::Text("Guid: %s", meta.GetFileGuid().string().c_str());
        ImGui::Separator();
    }
}
