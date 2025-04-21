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

    std::string icon = EditorIcon::ICON_FORDER;

    std::string name = icon + " " + folderPath.filename().string();

    bool isOpen = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);
    
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
    {
        _focusForder =
            UmFileSystem.GetContext<File::ForderContext>(folderPath);
    }
    if (isOpen)
    {
        for (const auto& entry : fs::directory_iterator(folderPath))
        {
            if (entry.is_directory())
            {
                ImGui::PushID(folderPath.c_str());

                ShowFolderHierarchy(entry.path());

                ImGui::PopID();
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

    if (ImGui::BeginDragDropTargetCustom(rect, window->ID))
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropTransform::KEY))
        {
            DragDropTransform::Data* data = (DragDropTransform::Data*)payload->Data;

            if (false == _focusForder.expired())
            {
                std::string path = _focusForder.lock()->GetPath().string();
                UmGameObjectFactory.WriteGameObjectFile(data->pTransform, path);
            }
        }
        ImGui::EndDragDropTarget();
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

        ImGui::Selectable(name.c_str());

        //2025_04_18 드래그 이벤트 추가
        if (ImGui::BeginDragDropSource())
        {
            DragDropAsset::Data data;
            data.Path = context->GetPath();
            ImGui::SetDragDropPayload(DragDropAsset::KEY, &data, sizeof(DragDropAsset::Data));
            ImGui::EndDragDropSource();
        }

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

void EditorFileObject::SetContext(std::weak_ptr<File::Context> context) 
{
    _context = context;
}
