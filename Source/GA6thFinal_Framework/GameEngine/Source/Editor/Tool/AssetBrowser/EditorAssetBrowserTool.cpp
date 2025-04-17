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
    _focusForder = UmFileSystem.GetContext<File::ForderContext>(
        File::Path(UmFileSystem.GetRootPath()));
}

void EditorAssetBrowserTool::OnPreFrame()
{
}

void EditorAssetBrowserTool::OnFrame()
{
    ImGui::PushID(this);

    BeginColum();

    // 왼쪽: 폴더 트리
    ImGui::BeginChild("FolderHierarchyView", ImVec2(0, 0), true);
    ShowFolderHierarchy(UmFileSystem.GetRootPath());
    ImGui::EndChild();

    ImGui::NextColumn();

    // 오른쪽: 선택한 폴더의 파일 목록
    ImGui::BeginChild("ContentsView", ImVec2(0, 0), true);
    ShowFolderContents();
    ImGui::EndChild();

    EndColum();

    ImGui::PopID();
}

void EditorAssetBrowserTool::OnPostFrame()
{
}

// 왼쪽: 폴더 트리 표시 (재귀)
void EditorAssetBrowserTool::ShowFolderHierarchy(const File::Path& folderPath)
{
    if (false == fs::exists(folderPath))
        return;

    bool isOpen = ImGui::TreeNodeEx(("##" + folderPath.string()).c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

    std::string icon = isOpen ? EditorIcon::ICON_FORDER_OPEN : EditorIcon::ICON_FORDER;
    std::string name = icon + " " + folderPath.filename().string();
    ImGui::SameLine();
    auto flags = ImGuiSelectableFlags_AllowDoubleClick;
    if (ImGui::Selectable((name).c_str(), isOpen, flags))
    {
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        _focusForder =
            UmFileSystem.GetContext<File::ForderContext>(folderPath);
    }
    if (isOpen)
    {
        icon = EditorIcon::ICON_FORDER_OPEN; // 트리 노드가 열렸을 때 아이콘을 오픈으로 변경

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
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImRect       rect   = window->Rect(); // 윈도우 전체 영역

    DragDropTransform::Data data;
    if (ImGuiHelper::DragDrop::RecieveFrameDragDropEvent(DragDropTransform::key, &data))
    {
        if (false == _focusForder.expired())
        {
            if (data.serializedFunc)
            {
                std::string path = _focusForder.lock()->GetPath().string();
                data.serializedFunc(path);
            }
        }
    }

    if (ImGui::BeginPopupContextItem())
    {
        ImGui::MenuItem("Show in explorer");
        ImGui::MenuItem("Refresh");
        // 팝업 내용
        ImGui::EndPopup();
    }
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
        auto spFocusCtx = _focusForder.lock();

        for (auto itr = spFocusCtx->begin(); itr != spFocusCtx->end(); ++itr)
        {
            auto& [name, wpFileCtx] = *itr;
            if (false == wpFileCtx.expired())
            {
                auto spFileCtx = wpFileCtx.lock();

                File::Path  path = spFileCtx->GetPath();
                std::string id   = name.string() + "##" + path.string();

                bool isMeta = path.extension() == UmFileSystem.GetMetaExt();

                if (true == mAssetBrowserFlags[ShowMetaFile] ||
                    (false == mAssetBrowserFlags[ShowMetaFile] &&
                     false == isMeta))
                {
                    ShowItemToList(spFileCtx);
                }
            }
        }
    }
}

void EditorAssetBrowserTool::ShowContentsToIcon() {}

void EditorAssetBrowserTool::ShowItemToList(spContext context) 
{
    const File::Path& path = context->GetPath();
    const std::string icon = context->IsDirectory() ? EditorIcon::ICON_FORDER : EditorIcon::ICON_FILE;
    const std::string name = icon + " " + context->GetName();

    bool isMeta = path.extension() == UmFileSystem.GetMetaExt();

    if (true == mAssetBrowserFlags[ShowMetaFile] || (false == mAssetBrowserFlags[ShowMetaFile] && false == isMeta))
    {
        ImGui::PushID(context.get());

        bool isSelected = _selectedContext->GetContext().lock() == context;

        ImGui::Selectable(name.c_str(), isSelected);

        if (ImGui::BeginPopupContextItem())
        {
            ImGui::MenuItem("Open##1");
            ImGui::MenuItem("Delete##2");
            // 팝업 내용
            ImGui::EndPopup();
        }

        ItemClickedAction(context);

        ImGui::PopID();
    }
}

void EditorAssetBrowserTool::ShowItemToIcon(spContext context) 
{

}

void EditorAssetBrowserTool::ItemClickedAction(spContext context) 
{
    if (ImGui::IsItemHovered())
    {
        // 더블 클릭 시 (이거 먼저 확인해야함!)
        if (ImGui::IsMouseDoubleClicked(0))
        {
            // 폴더일 시
            if (context->IsDirectory())
            {
                _focusForder = std::static_pointer_cast<File::ForderContext>(context);
            }
            // 파일일 시
            else
            {
            }
        }
        // 일반 클릭 시
        else if (ImGui::IsItemClicked(0))
        {
            _selectedContext->SetContext(context);
            EditorInspectorTool::SetFocusObject(_selectedContext);
        }
        else if (ImGui::IsItemClicked(1))
        {
            //ImGui::OpenPopup("RightClickPopup");
        }
    }
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