#include "pch.h"
#include "EditorAssetBrowserTool.h"

namespace fs = std::filesystem;

EditorAssetBrowserTool::EditorAssetBrowserTool()
{
    SetLabel("AssetBrowser");
    SetDockLayout(DockLayout::DOWN);

    _selectedContext = std::make_shared<EditorFileObject>();

    mShowType = List;
}

EditorAssetBrowserTool::~EditorAssetBrowserTool()
{
}

void EditorAssetBrowserTool::OnStartGui()
{
    _focusForder = File::FileSystem::GetContext<File::ForderContext>(
        Global::fileSystem->GetRootPath());
}

void EditorAssetBrowserTool::OnPreFrame()
{
}

void EditorAssetBrowserTool::OnFrame()
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

void EditorAssetBrowserTool::OnPostFrame()
{
}

// 왼쪽: 폴더 트리 표시 (재귀)
void EditorAssetBrowserTool::ShowFolderHierarchy(const File::Path& folderPath)
{
    std::string id =
        folderPath.filename().string() + "##" + folderPath.string();

    bool isOpen = ImGui::TreeNodeEx(id.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
    
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        _focusForder =
            File::FileSystem::GetContext<File::ForderContext>(folderPath);
    }
    if (isOpen)
    {
        for (const auto& entry : fs::directory_iterator(folderPath))
        {
            if (entry.is_directory())
            {
                ShowFolderHierarchy(entry.path());
            }
        }
        ImGui::TreePop();
    }
}

// 오른쪽: 선택된 폴더의 파일 목록
void EditorAssetBrowserTool::ShowFolderContents()
{
    switch (mShowType)
    {
    case EditorAssetBrowserTool::List:
        ShowContentsToList();
        break;
    case EditorAssetBrowserTool::Icon:
        ShowContentsToIcon();
        break;
    default:
        break;
    }
}

void EditorAssetBrowserTool::ShowContentsToList()
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
                        EditorInspectorTool::SetFocusObject(_selectedContext);
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

void EditorAssetBrowserTool::ShowContentsToIcon()
{
}


void EditorAssetBrowserTool::BeginColum()
{
    ImGui::Columns(2, "AssetBrowser", true);
    //ImGui::SetColumnWidth(0, mPanelWidth);
}

void EditorAssetBrowserTool::EndColum()
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

void EditorFileObject::SetContext(std::weak_ptr<File::Context> context) 
{
    _context = context;
}
