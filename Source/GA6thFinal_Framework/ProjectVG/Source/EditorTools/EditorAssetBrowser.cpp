#include "EditorAssetBrowser.h"

namespace fs = std::filesystem;

EditorAssetBrowser::EditorAssetBrowser()
{
    SetLabel("AssetBrowser");
    SetInitialDockSpaceArea(DockSpaceArea::DOWN);

    mShowType = List;
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

// 왼쪽: 폴더 트리 표시 (재귀)
void EditorAssetBrowser::ShowFolderHierarchy(const Path& folderPath)
{
    for (const auto& entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_directory()) 
        {
            std::string id = entry.path().filename().string() + "##" + entry.path().string();
            
            bool isOpen = ImGui::TreeNodeEx(id.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
            //auto flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
            //bool isOpen = ImGui::BeginTable("3ways", 3, flags);
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                mFocusFolderPath = entry.path();
            }
            if (isOpen)
            {
                ShowFolderHierarchy(entry.path()); // 재귀적으로 하위 폴더 표시
                ImGui::TreePop();
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

void EditorAssetBrowser::OnFrame()
{
    BeginColum();

    // 왼쪽: 폴더 트리
    ImGui::BeginChild("FolderHierarchyView", ImVec2(0, 0), true);
    ShowFolderHierarchy("Assets");
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

void EditorAssetBrowser::ShowContentsToList()
{
    //for (const auto& entry : fs::directory_iterator(mFocusFolderPath))
    //{
    //    std::string id = entry.path().filename().string() + "##" + entry.path().string();
    //    // 파일이 유효한 확장자일 경우 표시
    //    if (entry.is_regular_file())
    //    {
    //        if (mAssetBrowserFlags[ShowMetaFile] == true ||
    //            (mAssetBrowserFlags[ShowMetaFile] == false && !Asset::System::IsMetaFile(entry.path())))
    //        {
    //            if (ImGui::Selectable(id.c_str()))
    //            {
    //                mSelectedEntry = entry.path();
    //            }
    //            // 더블 클릭시 파일 열기
    //            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    //            {
    //                ShellExecute(NULL, L"open", mSelectedEntry.wstring().c_str(), NULL, NULL, SW_SHOW);
    //            }
    //        }
    //    }
    //    // 폴더인 경우 누를 시 그 경로로 이동
    //    if (entry.is_directory())
    //    {
    //        if (ImGui::Selectable(id.c_str()))
    //        {
    //            mSelectedEntry = entry.path();
    //        }
    //        // 더블 클릭시 폴더 열기
    //        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    //        {
    //            mFocusFolderPath = entry.path();
    //            //ImGui::IMGUI_DEMO_MARKER("Examples/Assets Browser");
    //        }
    //    }
    //}
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
